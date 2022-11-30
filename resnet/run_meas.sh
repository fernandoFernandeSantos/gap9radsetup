NE16=1
SEC_FLASH=1

MAIN=main.c
PROJ_NAME=resnet
MODEL_SIZE=50
MODEL_TYPE=fp32
LOG_DIR=log_meas

SUFF="sq8"
if [ ${NE16} -eq 1 ]
then
	SUFF="ne16"
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

touch ${MAIN}
make_cmd="make MODEL_SIZE=${MODEL_SIZE} MODEL_TYPE=${MODEL_TYPE} USE_PRIVILEGED_FLASH=${SEC_FLASH} MODEL_NE16=${NE16}"
echo ${make_cmd}
${make_cmd} clean_model model > ${LOG_DIR}/${PROJ_NAME}_${MODEL_SIZE}_${SUFF}_at.log
${make_cmd} io=uart all -j

# High Performance
F=370
V=800
python $GAP_SDK_HOME/utils/power_meas_utils/ps4444Measure.py ${LOG_DIR}/${PROJ_NAME}_${MODEL_SIZE}_${SUFF}_${F}MHz_${V}mV & touch ${MAIN} && \
${make_cmd} GPIO_MEAS=1 FREQ_CL=${F} FREQ_FC=${F} FREQ_PE=${F} VOLTAGE=${V} io=uart run
wait_finished_job

# Energy Efficient
F=240
V=650
python $GAP_SDK_HOME/utils/power_meas_utils/ps4444Measure.py ${LOG_DIR}/${PROJ_NAME}_${MODEL_SIZE}_${SUFF}_${F}MHz_${V}mV & touch ${MAIN} && \
${make_cmd} GPIO_MEAS=1 FREQ_CL=${F} FREQ_FC=${F} FREQ_PE=${F} VOLTAGE=${V} io=uart run
wait_finished_job
