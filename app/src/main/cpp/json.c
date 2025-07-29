#include "common.h"
#include "query.h"
#include "uic.h"

struct eom_result {
	int lane;
	int timing;
	int volt;
	int error_cnt;
};

struct EOMData {
	int timing_max_steps;
	int timing_max_offset;
	int voltage_max_steps;
	int voltage_max_offset;
	int data_cnt;
	int num_lanes;
	int local_peer;
	int gear;
	int rate;

	struct eom_result *er;
};

extern int get_device_info(char *mname, char *pname, char *pversion);

int generate_json_report(char *eom_file, struct EOMData *data)
{
	char mname[MANUFACTURER_NAME_STRING_DESC_SIZE];
	char pname[PRODUCT_NAME_STRING_DESC_SIZE];
	char pver[PRODUCT_REVISION_LEVEL_STRING_DESC_SIZE];
	char eom_report_name[1024];
	int i, ret;
	float timing_step_size, voltage_step_size;
	FILE *file;

	strcpy(eom_report_name, eom_file);
	strcat(eom_report_name, ".json");
	file = fopen(eom_report_name, "w");
	if (file == NULL) {
		pr_err("Failed to create json file %s\n", eom_report_name);
		return ERROR;
	}

	ret = get_device_info(mname, pname, pver);
	if (ret)
		return ret;

	timing_step_size = (float)data->timing_max_offset * 0.01 / (float)data->timing_max_steps;
	voltage_step_size = (float)data->voltage_max_offset * 10 / (float)data->voltage_max_steps;

	fprintf(file, "EOM Capabilities:\n");
	fprintf(file, "TimingMaxSteps %d TimingMaxOffset %d\n", data->timing_max_steps, data->timing_max_offset);
	fprintf(file, "VoltageMaxSteps %d VoltageMaxOffset %d\n\n", data->voltage_max_steps, data->voltage_max_offset);
	fprintf(file, "UFS Eye Monitor - Data Collection In Progress\n");
	fprintf(file, "[EOMREPORTLIB]{\n");
	fprintf(file, "[EOMREPORTLIB]   \"version\": \"1.0.0\",\n");
	fprintf(file, "[EOMREPORTLIB]   \"results\": [\n");
	fprintf(file, "[EOMREPORTLIB]         {\n");
	fprintf(file, "[EOMREPORTLIB]            \"interface\": \"%s UFS\",\n", data->local_peer ? "Device" : "Host");
	fprintf(file, "[EOMREPORTLIB]            \"instance\": 0,\n");
	fprintf(file, "[EOMREPORTLIB]            \"time_scale\": %f,\n", timing_step_size);
	fprintf(file, "[EOMREPORTLIB]            \"time_units\": \"UI\",\n");
	fprintf(file, "[EOMREPORTLIB]            \"voltage_scale\": %f,\n", voltage_step_size);
	fprintf(file, "[EOMREPORTLIB]            \"voltage_units\": \"mV\",\n");
	fprintf(file, "[EOMREPORTLIB]            \"note\": \"%s %s %s\",\n", mname, pname, pver);
	fprintf(file, "[EOMREPORTLIB]            \"lanes\": [\n");
	fprintf(file, "[EOMREPORTLIB]                  {\n");
	fprintf(file, "[EOMREPORTLIB]                  \"lane_number\": 0,\n");
	fprintf(file, "[EOMREPORTLIB]                  \"note\": \"\",\n");
	fprintf(file, "[EOMREPORTLIB]                  \"eye\": [\n");

	for (i = 0; i < data->data_cnt; i++) {
		fprintf(file, "[EOMREPORTLIB]                     [%d, %d, %d]",
				data->er[i].timing, data->er[i].volt, data->er[i].error_cnt);

		/* Append specified strings at the end of each lane to meet the Jason format */
		if (data->num_lanes == 2 && i == (data->data_cnt/2 - 1)) {
			fprintf(file, "%s", "\n");
			fprintf(file, "[EOMREPORTLIB]                     ]\n");
			fprintf(file, "[EOMREPORTLIB]                  },\n");
			fprintf(file, "[EOMREPORTLIB]                  {\n");
			fprintf(file, "[EOMREPORTLIB]                  \"lane_number\": 1,\n");
			fprintf(file, "[EOMREPORTLIB]                  \"note\": \"\",\n");
			fprintf(file, "[EOMREPORTLIB]                  \"eye\": [\n");
		} else {
			fprintf(file, "%s", (i == data->data_cnt-1) ? "\n" : ",\n");
		}
	}

	fprintf(file, "[EOMREPORTLIB]                     ]\n");
	fprintf(file, "[EOMREPORTLIB]                  }\n");
	fprintf(file, "[EOMREPORTLIB]               ]\n");
	fprintf(file, "[EOMREPORTLIB]         }\n");
	fprintf(file, "[EOMREPORTLIB]      ]\n");
	fprintf(file, "[EOMREPORTLIB]}\n");

	fclose(file);
	printf("EOM results saved to %s\n", eom_report_name);

	return SUCCESS;
}
