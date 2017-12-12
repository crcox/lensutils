const char *argp_program_version = "netacc 0.1";

const char *argp_program_bug_address = "<cox.crc@gmail.com>";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  int index;
  int rank;
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
  {"rank", 'r', 0, 0, "Output how many alternatives were closer to target than output pattern."},
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
    case 'r':
      arguments->rank = 1;
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
"netacc -- A program to determine whether each example's output pattern is closer to it's target than anything else.";

/*
   The ARGP structure itself.
*/
static struct argp argp = {options, parse_opt, args_doc, doc};

