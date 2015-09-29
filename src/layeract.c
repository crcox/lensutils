#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include "tcl.h"
#include "lens.h"
#include "util.h"
#include "network.h"

const char *argp_program_version =
"layeract 0.1";

const char *argp_program_bug_address =
"<cox.crc@gmail.com>";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  int index;
  int verbose;
  int tick;
  char *args[4];
};

/*
   OPTIONS.  Field 1 in ARGP.
   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
  {"index", 'i', 0, 0, "Prepend example number to each row of output."},
  {"verbose", 'v', 0, 0, "Print input and counts to stderr."},
  {"tick", 't', "NUMBER", 0, "Report activations at a particular tick"},
  {0}
};


/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'i':
      arguments->index = 1;
      break;
    case 'v':
      arguments->verbose = 1;
      break;
    case 't':
      arguments->tick = atoi(arg);
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 4)
      {
        argp_usage(state);
      }
      arguments->args[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 4)
      {
        argp_usage (state);
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/*
   ARGS_DOC. Field 3 in ARGP.
   A description of the non-option command-line arguments
     that we accept.
*/
static char args_doc[] = "NETWORK LAYER EXAMPLES WEIGHTS";

/*
  DOC.  Field 4 in ARGP.
  Program documentation.
*/
static char doc[] =
"layeract -- A program to report the activation over a layer of a Lens neural network. \vOne of the LensUtilities.";

/*
   The ARGP structure itself.
*/
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[]) {
  struct arguments arguments;
  int nexamples;
  int nunits;
  int nticks;
  int gn;
  int tickstart=0;
  double unitact;
  double unittarg;

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
  nexamples=Root->set[0]->numExamples;
  /* Figure out the group number of associated with layer name */
  lens("path -group %s", layername);
  sscanf(Tcl_GetStringResult(Interp), "group(%d)", &gn);
  /* Number of units in layer */
  nunits = Net->group[8]->numUnits;

  if (arguments.verbose) {
    fprintf(stderr,"Network:        %s\n", netfilename);
    fprintf(stderr,"Layer    (%d):  %s(%d)\n",nunits,layername,gn);
    fprintf(stderr,"Examples (%d): %s\n", nexamples, examplefilename);
    fprintf(stderr,"Weights:        %s\n",weightfilename);
    fprintf(stderr,"-t:        %d\n",arguments.tick);
  }

  for (int iex=0; iex<nexamples; iex++) {
    if(arguments.index==1) {fprintf(stdout,"%d ",iex);}
    lens("doExample %d -set testset -test", iex);
    nticks = Net->ticksOnExample;
    if (nticks>1){tickstart=1;}
    if (arguments.tick>0) {
      tickstart=arguments.tick;
      nticks = tickstart+1;
    }
    if(arguments.verbose) {fprintf(stderr,"Ticks: %d\n",nticks);}

    for (int itick=tickstart; itick<nticks; itick++) {
      if(arguments.index==1) {fprintf(stdout,"%d ",itick);}
      for (int i=0;i<nunits; i++) {
        unittarg = Net->group[gn]->unit[i].targetHistory[itick];
        unitact = Net->group[gn]->unit[i].outputHistory[itick];
        fprintf(stdout,"%.3f ",unitact);
      }
      fprintf(stdout,"\n");
    }
  }
  return 0;
}
