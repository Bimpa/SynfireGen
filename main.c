

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

unsigned int N = 31;

#define NEURON_LOG_INDEX 		2
#define SIMULATION_LOG_END 		1.0
#define SIMULATION_LOG_INTERVAL 0.001//0.001

#define DEVICE_LOGDATA_DIRECTORY "/home/paul/data/code/SVN/C/spinnaker/device/damson_loader/devicelogs/"
#define EMULATOR_LOGDATA_DIRECTORY "/home/paul/data/code/SVN/C/damson/"

/* --------------------------------------------------------- */
int main(int argc, char *argv[]){
	unsigned int y;
	FILE *out;
	char aliasFilename[] = "synfire_alias.d";
	char plotESpikeFilename[] = "plot_E_spike.gnu";
	char plotEVFilname[] = "plot_E_v.gnu";


	/* alias first */
	out = fopen(aliasFilename, "w");

	if (out == NULL) {
		fprintf(stderr, "Can't open output file %s!\n", aliasFilename);
		exit(1);
	}

	//srand (time(NULL));
	srand(1234);

	printf("Printing Alias File...\n");

	fprintf(out, "#define NEURON_LOG_INDEX  %u\n", NEURON_LOG_INDEX);

	fprintf(out, "#alias PulsePacket  %u\n",1);
	fprintf(out, "seed=%u;\n", rand());
	fprintf(out, "clock_int: 0\n");
	fprintf(out, "#snapshot \"spike\" 0.0 %f %f \"%%f %%d\\n\" t spike_index\n", SIMULATION_LOG_END, SIMULATION_LOG_INTERVAL);
	fprintf(out, "\n");

	for (y = 1; y<=N; y++){
			//output excitatory alias
			fprintf(out, "#alias SynFireE %u\n", y*2);
			fprintf(out, "seed=%u;\n", rand());
			fprintf(out, "clock_int: 0\n");
			if (y == 1)
				fprintf(out, "E_to_E_spike: %u\n", 1);
			else
				fprintf(out, "E_to_E_spike: %u\n", ((y-1)*2));
			fprintf(out, "I_to_E_spike: %u\n", (y*2)+1);
			//fprintf(out, "#snapshot \"spike\" 0.0 %f %f \"%%f %%d\\n\" t spike_index\n", SIMULATION_LOG_END, SIMULATION_LOG_INTERVAL);
			//fprintf(out, "#log \"v\" 0.0 %f %f \"%%f  %%f\\n\" t  v[NEURON_LOG_INDEX] \n", SIMULATION_LOG_END, SIMULATION_LOG_INTERVAL);
			fprintf(out, "\n");

			//output inhibitory alias
			fprintf(out, "#alias SynFireI %u\n", (y*2)+1);
			fprintf(out, "seed=%u;\n", rand());
			fprintf(out, "clock_int: 0\n");
			if (y == 1)
				fprintf(out, "E_to_I_spike: %u\n", 1);
			else
				fprintf(out, "E_to_I_spike: %u\n", ((y-1)*2));
			//fprintf(out, "#snapshot \"spike\" 0.0 %f %f \"%%f %%d\\n\" t spike_index\n", SIMULATION_LOG_END, SIMULATION_LOG_INTERVAL);
			//fprintf(out, "#log \"v\" 0.0 %f %f \"%%f  %%f\\n\" t  v[NEURON_LOG_INDEX] \n", SIMULATION_LOG_END, SIMULATION_LOG_INTERVAL);
			fprintf(out, "\n");
	}

	fclose(out);

	/* spike plot*/
	printf("Printing Spike Raster Plot Script...\n");
	out = fopen(plotESpikeFilename, "w");
	if (out == NULL) {
		fprintf(stderr, "Can't open output file %s!\n", plotESpikeFilename);
		exit(1);
	}
	fprintf(out, "EXCITATORY_NEURONS = 50\n");

	fprintf(out, "set term wxt 1\n");
	fprintf(out, "set title \"Spike Raster Plot Excitatory Populations (SpiNNaker)\"\n");
	fprintf(out, "set xlabel \"Time (ms)\"\n");
	fprintf(out, "set ylabel \"neuron index\"\n");
	fprintf(out, "set grid\n");
	fprintf(out, "unset key\n");
	fprintf(out, "plot \"%ssynfire_PulsePacket_1_spike.dat\" using ($1): ($2+(0*EXCITATORY_NEURONS)) with points pointsize 1 pointtype 7, \\\n", DEVICE_LOGDATA_DIRECTORY);
	for (y = 1; y<=N; y++){
		fprintf(out, "     \"%ssynfire_SynFireE_%d_spike.dat\" using ($1): ($2+(%d*EXCITATORY_NEURONS)) with points pointsize 1 pointtype 7", DEVICE_LOGDATA_DIRECTORY, (y*2), y);
		if (y != N)
			fprintf(out, ", \\\n");
		else
			fprintf(out, "\n");
	}

	fprintf(out, "set term wxt 2\n");
	fprintf(out, "set title \"Spike Raster Plot Excitatory Populations (Emulation)\"\n");
	fprintf(out, "set xlabel \"Time (ms)\"\n");
	fprintf(out, "set ylabel \"neuron index\"\n");
	fprintf(out, "set grid\n");
	fprintf(out, "unset key\n");
	fprintf(out, "plot \"%ssynfire_PulsePacket_1_spike.dat\" using ($1): ($2+(0*EXCITATORY_NEURONS)) with points pointsize 1 pointtype 7, \\\n", EMULATOR_LOGDATA_DIRECTORY);
	for (y = 1; y<=N; y++){
		fprintf(out, "     \"%ssynfire_SynFireE_%d_spike.dat\" using ($1): ($2+(%d*EXCITATORY_NEURONS)) with points pointsize 1 pointtype 7", EMULATOR_LOGDATA_DIRECTORY, (y*2), y);
		if (y != N)
			fprintf(out, ", \\\n");
		else
			fprintf(out, "\n");
	}
	fclose(out);

	/* line plot*/
	printf("Printing Excitatory Voltage Line Plot Script...\n");
	out = fopen(plotEVFilname, "w");
	if (out == NULL) {
		fprintf(stderr, "Can't open output file %s!\n", plotEVFilname);
		exit(1);
	}
	fprintf(out, "set title \"Membrane Potential Plot of Excitatory Populations (1000Hz Tickrate)\"\n");
	fprintf(out, "set xlabel \"Time (ms)\"\n");
	fprintf(out, "set ylabel \"v (mV)\"\n");
	fprintf(out, "set grid\n");
	fprintf(out, "plot ");
	for (y = 1; y<=N; y++){
		fprintf(out, "     \"%ssynfire_SynFireE_%d_v.dat\" with lines title \"Population %d SpiNNaker Excitatory Neuron [%d]\", \\\n", DEVICE_LOGDATA_DIRECTORY, (y*2), (y*2), NEURON_LOG_INDEX);
		fprintf(out, "     \"%ssynfire_SynFireE_%d_v.dat\" with lines title \"Population %d Emulator Excitatory Neuron [%d]\"", EMULATOR_LOGDATA_DIRECTORY, (y*2), (y*2), NEURON_LOG_INDEX);
		if (y != N)
			fprintf(out, ", \\\n");
		else
			fprintf(out, "\n");
	}
	fclose(out);

	printf("Done.\n");

	return 0;
}
