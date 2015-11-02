#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include "tcl.h"
#include "system.h"
#include "util.h"
#include "network.h"
#include "example.h"
#include "lens.h"
#include "layeract.h"

real * layeract(char *netfilename, char *layername, char *examplefilename, char *weightfilename, int tick)
{
  int nexamples;
  int nunits;
  int nticks;
  int tickstart=0;
  real *A;
  Group G;

  lens("source %s", netfilename);
  lens("loadWeights %s", weightfilename);
  lens("loadExamples %s -set testset -exmode ORDERED", examplefilename);
  lens("useTestingSet testset");
  nexamples=Net->testingSet->numExamples;
  G = lookupGroup(layername);
  nunits = G->numUnits;
  nticks = Net->ticksOnExample;
  A = realArray(nexamples*nunits*nticks,"name of array");

  for (int iex=0; iex<nexamples; iex++) {
    lens("doExample %d -set testset -test", iex);
    if (nticks>1){tickstart=1;}
    if (tick>0) {
      tickstart=tick;
      nticks = tickstart+1;
    }

    for (int itick=tickstart; itick<nticks; itick++) {
      for (int i=0;i<nunits; i++) {
        A++;
        A = &G->unit[i].outputHistory[itick];
      }
      fprintf(stdout,"\n");
    }
  }
  return A;
}

int main(int argc, char *argv[]) {
  struct arguments arguments;

  if (startLens(argv[0], 1)) {
    fprintf(stderr, "Unable to start Lens... exiting.\n");
    exit(1);
  }

  /* Set argument defaults */
  arguments.index = 0;
  arguments.verbose = 0;
  arguments.tick = -1;

  /* Where the magic happens */
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  char *netfilename= arguments.args[0];
  char *layername = arguments.args[1];
  char *examplefilename = arguments.args[2];
  char *weightfilename = arguments.args[3];

  if (arguments.verbose) {
    fprintf(stderr,"Network :        %s\n", netfilename);
    fprintf(stderr,"Layer :          %s\n", layername);
    fprintf(stderr,"Examples :       %s\n", examplefilename);
    fprintf(stderr,"Weights :        %s\n", weightfilename);
    fprintf(stderr,"-t :             %d\n",arguments.tick);
  }

  layeract(netfilename, layername, examplefilename, weightfilename, arguments.tick);

  return 0;
}
