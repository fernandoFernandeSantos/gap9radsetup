ID=0
NE16=1
START_MODEL=0
STOP_MODEL=4
SEC_FLASH=1
MODEL_TYPE=tflite
LOG_DIR=log_meas

SUFF="_sq8"
if [ ${NE16} -eq 1 ]
then
	SUFF="_ne16"
fi

wait_finished_job() {
	if [ $? -eq "1" ]; then # kill the measurement job
		for job in `jobs -p`
		do
			echo $job
			kill -9 $job
		done
		continue
	else # wait measurment job
		for job in `jobs -p`
		do
			echo $job
			wait $job
		done
	fi
}

for ID in `seq $START_MODEL $STOP_MODEL`;
do
	touch efficientnet_lite.c
	make_cmd="make MODEL_SIZE=${ID} MODEL_TYPE=${MODEL_TYPE} USE_PRIVILEGED_FLASH=${SEC_FLASH} MODEL_NE16=${NE16}"
	echo ${make_cmd}
	${make_cmd} clean_model model > ${LOG_DIR}/efficientnet_lite_${ID}${SUFF}_at.log
	${make_cmd} io=uart all -j

	# High Performance
	F=370
	V=800
	python $GAP_SDK_HOME/utils/power_meas_utils/ps4444Measure.py ${LOG_DIR}/efficientnet_lite_${ID}${SUFF}_${F}MHz_${V}mV & touch efficientnet_lite.c && \
	${make_cmd} GPIO_MEAS=1 FREQ_CL=${F} FREQ_FC=${F} FREQ_PE=${F} VOLTAGE=${V} io=uart run
	wait_finished_job

	# Energy Efficient
	F=240
	V=650
	python $GAP_SDK_HOME/utils/power_meas_utils/ps4444Measure.py ${LOG_DIR}/efficientnet_lite_${ID}${SUFF}_${F}MHz_${V}mV & touch efficientnet_lite.c && \
	${make_cmd} GPIO_MEAS=1 FREQ_CL=${F} FREQ_FC=${F} FREQ_PE=${F} VOLTAGE=${V} io=uart run
	wait_finished_job
done
