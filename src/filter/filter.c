#include "includes.h"
#include "gyro.h"
#include "filter.h"
#include "kalman.h"
#include "biquad.h"

volatile filter_config_t filterConfig = {
		DEFAULT_ROLL_Q,
		DEFAULT_PITCH_Q,
		DEFAULT_YAW_Q,
		MIN_WINDOW_SIZE,
		(float)DEFAULT_ROLL_Q,
		(float)DEFAULT_PITCH_Q,
		(float)DEFAULT_YAW_Q,
		(float)BASE_LPF_HZ,
		(float)BASE_LPF_HZ,
		(float)BASE_LPF_HZ,
};

biquad_state_t lpfFilterStateRate;
volatile uint32_t setPointNew;
volatile axisDataInt_t setPointInt;
volatile axisData_t oldSetPoint;
volatile axisData_t setPoint;
volatile int allowFilterInit = 1;

void allow_filter_init(void)
{
	allowFilterInit = 1;
}

void filter_biquad_init(float freq, biquad_axis_state_t *filterState)
{
	biquad_init(freq, filterState, REFRESH_RATE, FILTER_TYPE_LOWPASS, BIQUAD_BANDWIDTH);
}

void filter_init(void)
{
	memset((uint32_t *)&setPoint, 0, sizeof(axisData_t));
	memset((uint32_t *)&oldSetPoint, 0, sizeof(axisData_t));
	memset((uint32_t *)&setPointInt, 0, sizeof(axisDataInt_t));
	kalman_init();
	filter_biquad_init(BASE_LPF_HZ, &(lpfFilterStateRate.x));
	filter_biquad_init(BASE_LPF_HZ, &(lpfFilterStateRate.y));
	filter_biquad_init(BASE_LPF_HZ, &(lpfFilterStateRate.z));
}

void filter_data(volatile axisData_t *gyroRateData, volatile axisData_t *gyroAccData, float gyroTempData, filteredData_t *filteredData)
{
	if (allowFilterInit)
	{
		allowFilterInit = 0;
		//convert the ints to floats
		filterConfig.roll_q = (float)filterConfig.i_roll_q;
		filterConfig.pitch_q = (float)filterConfig.i_pitch_q;
		filterConfig.yaw_q = (float)filterConfig.i_yaw_q;
		filter_init();
	}

	if (setPointNew)
	{
		memcpy((uint32_t *)&setPoint, (uint32_t *)&setPointInt, sizeof(axisData_t));
	}

	kalman_update(gyroRateData, filteredData);

	filteredData->rateData.x = biquad_update(filteredData->rateData.x, &(lpfFilterStateRate.x));
	filteredData->rateData.y = biquad_update(filteredData->rateData.y, &(lpfFilterStateRate.y));
	filteredData->rateData.z = biquad_update(filteredData->rateData.z, &(lpfFilterStateRate.z));

	if (setPointNew)
	{
		setPointNew = 0;
		if (setPoint.x != 0.0f && oldSetPoint.x != setPoint.x)
		{
			filterConfig.roll_lpf_hz = CONSTRAIN(BASE_LPF_HZ * ABS(1.0f - (setPoint.x / filteredData->rateData.x)), 10.0f, 500.0f);
			filter_biquad_init(filterConfig.roll_lpf_hz, &(lpfFilterStateRate.x));
		}
		if (setPoint.y != 0.0f && oldSetPoint.y != setPoint.y)
		{
			filterConfig.pitch_lpf_hz = CONSTRAIN(BASE_LPF_HZ * ABS(1.0f - (setPoint.y / filteredData->rateData.y)), 10.0f, 500.0f);
			filter_biquad_init(filterConfig.pitch_lpf_hz, &(lpfFilterStateRate.y));
		}
		if (setPoint.z != 0.0f && oldSetPoint.z != setPoint.z)
		{
			filterConfig.yaw_lpf_hz = CONSTRAIN(BASE_LPF_HZ * ABS(1.0f - (setPoint.z / filteredData->rateData.z)), 10.0f, 500.0f);
			filter_biquad_init(filterConfig.yaw_lpf_hz, &(lpfFilterStateRate.z));
		}
		memcpy((uint32_t *)&oldSetPoint, (uint32_t *)&setPoint, sizeof(axisData_t));
	}
	//no need to filter ACC is used in quaternions
	filteredData->accData.x = gyroAccData->x;
	filteredData->accData.y = gyroAccData->y;
	filteredData->accData.z = gyroAccData->z;

	//should filter this
	filteredData->tempC = gyroTempData;
}