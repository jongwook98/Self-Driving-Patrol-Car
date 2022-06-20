/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/core/core.h>

#include <argp.h>
#include <unistd.h>

static error_t parse_opt(int key, char *arg, struct argp_state *state);
static int do_arguments(int argc, char *argv[]);

const char *argp_program_bug_address =
    "<qkrtkdwo76@gmail.com> and <dbdnjs9804@gmail.com>";

int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);

  // set log to stderr
  fLB::FLAGS_logtostderr = true;

  // set log level to ERROR
  fLI::FLAGS_minloglevel = 2;

  do_arguments(argc, argv);

  Core core;
  core.Run();

  google::ShutdownGoogleLogging();

  return 0;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  (void)state;

  if (key != 'l')
    return ARGP_ERR_UNKNOWN;

  fLI::FLAGS_minloglevel = strtol(arg, NULL, 10);
  return 0;
}

static int do_arguments(int argc, char *argv[]) {
  const struct argp_option options[] = {
      {"loglevel", 'l', "LEVEL", 0, "Set log-level (0:min to 3:max)"},
      {NULL},
  };
  const char args_doc[] = "";
  const char prog_doc[] = "lane_detection for target system";
  struct argp argp = {options, parse_opt, args_doc, prog_doc};

  argp_parse(&argp, argc, argv, 0, 0, NULL);
  return 0;
}
