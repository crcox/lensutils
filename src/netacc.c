#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <argp.h>
#include "tcl.h"
#include "lens.h"
#include "util.h"
#include "network.h"
#include "object.h"
#include "netacc.h"
float loopOverUnits(struct network *Net, int gn, int nunits, int iex, int jex) {
  float unitact, unittarg;
  float dist = 0;
  int i;
  for (i=0;i<nunits; i++) {
    unittarg = Net->testingSet->example[jex]->event[0].target->val[i];
    unitact = Net->group[gn]->unit[i].output;
    dist = dist + pow(unitact-unittarg, 2);
  }
  return(dist);
}
void loopOverTicksAndExamples(struct network *Net, int gn, int nunits, int nexamples, int iex, int tickstart, int nticks) {
  int acc;
  int i;
  int jex;
  int itick;
  int nevents;
  float unitact, unittarg;
  float dist, selfdist, otherdist, minotherdist;

  nevents = Net->testingSet->example[iex]->numEvents;
  for (itick=tickstart; itick<nticks; itick++) {
    // Skip over ticks belonging to events without targets
    if (Net->eventHistory[itick]!=(nevents-1)) {continue;}
    // Print tick index if asked for
    //if(arguments.index==1) {fprintf(stdout,"%d ",itick);}

    // What we want to know is: are the activations for this example closer
    // to the intended target than the target activation associated with any
    // other esample?
    minotherdist = 9999;
    selfdist = 0;
    acc = 0;
    for (jex=0; jex<nexamples; jex++) {
      otherdist = 0;
      for (i=0;i<nunits; i++) {
        unittarg = Net->testingSet->example[jex]->event[nevents-1].target->val[i];
        unitact = Net->group[gn]->unit[i].outputHistory[itick];
        dist = pow(unitact-unittarg, 2);
        if (iex==jex) {
          selfdist = selfdist + dist;
        } else {
          otherdist = otherdist + dist;
        }
      }
      if (iex!=jex && otherdist < minotherdist) {
        minotherdist = otherdist;
     }
    }
    if (selfdist < minotherdist) {acc = 1;}
    fprintf(stdout,"%d ",acc);
  }
}
    //minotherdist = otherdist; && otherdist < minotherdist
void loopOverExamples(struct network *Net, int gn, int nunits, int nexamples, int iex, int ReturnRank) {
  int jex;
  int acc, rank;
  float selfdist, otherdist;
  rank = 0;
  acc = 0;
  selfdist = loopOverUnits(Net, gn, nunits, iex, iex);
  for (jex=0; jex<nexamples; jex++) {
    otherdist = 0;
    if (iex!=jex) {
      otherdist = loopOverUnits(Net, gn, nunits, iex, jex);
      if ( otherdist < selfdist ) {
        rank++;
      }
    }
  }
  if (rank == 0) {acc = 1;}
  if (ReturnRank==1) fprintf(stdout,"%d",rank);
  else fprintf(stdout,"%d",acc);
}
int main(int argc, char *argv[]) {
  struct arguments arguments;
  int gn;
  int nticks;
  int tickstart=0;

  /* Set argument defaults */
  arguments.index = 0;
  arguments.rank = 0;
  arguments.verbose = 0;
  arguments.tick = -1;

  /* Where the magic happens */
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  int ReturnRank = arguments.rank;
  char *netfilename= arguments.args[0];
  char *layername = arguments.args[1];
  char *examplefilename = arguments.args[2];
  char *weightfilename = arguments.args[3];

  if (startLens(argv[0], 1)) {
    fprintf(stderr, "Lens Failed\n");
    exit(1);
  }

  /* Source the network file */
  lens("source %s", netfilename);

  /* Load the weights */
  lens("loadWeights %s", weightfilename);
  /* Load the examples */
  lens("loadExamples %s -set testset -exmode ORDERED", examplefilename);
  lens("useTestingSet testset");
  /* Number of examples */
  const int nexamples=Net->testingSet->numExamples;

  /* Figure out the group number of associated with layer name */
  lens("path -group %s", layername);
  sscanf(Tcl_GetStringResult(Interp), "group(%d)", &gn);
  /* Number of units in layer */
  const int nunits = Net->group[gn]->numUnits;

  if (arguments.verbose) {
    fprintf(stderr,"Network:        %s\n", netfilename);
    fprintf(stderr,"Layer    (%d):  %s(%d)\n",nunits,layername,gn);
    fprintf(stderr,"Examples (%d): %s\n", nexamples, examplefilename);
    fprintf(stderr,"Weights:        %s\n",weightfilename);
    fprintf(stderr,"-t:        %d\n",arguments.tick);
  }

  /* Iterator variables */
  int iex;
  for (iex=0; iex<nexamples; iex++) {
    // We will assume for now that the last event is where targets are //
    lens("doExample %d -set testset -test", iex);

    if(arguments.index==1) {fprintf(stdout,"%d ",iex);}
    if (Net->type & (1 << 1)) {
      nticks = Net->ticksOnExample;
      // Print example index if asked for
      // In continuous networks (where nticks>1) tick[0] is used as setup, and is
      // not intersting.
      if (nticks>1){tickstart=1;}
      // If a specific tick is specified "loop" over just that tick.
      if (arguments.tick>0) {
        tickstart=arguments.tick;
        nticks = tickstart+1;
      }
      loopOverTicksAndExamples(Net, gn, nunits, nexamples, iex, tickstart, nticks);
    } else {
      loopOverExamples(Net, gn, nunits, nexamples, iex, ReturnRank);
    }

    fprintf(stdout,"\n");
  }
  return 0;
}
