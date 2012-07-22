#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>     //exit

#include "prompt.h"
#include "management.h"
#include "services.h"
#include "nodes.h"
#include "algorithms.h"

//
//  Global variables
//

int Verbose = 0;
FILE *ReadFile = NULL;

//
//  Main function for the arbitrator
//  used for both the server and arbitrator
//

void *prompt_user(void *input)
{
  char Cmdline[512];
  int cargc;
  char *cargv[32];

  printf("Welcome to Arbitrator v0.0\n");

  //
  //  Loop through the command shell
  //

  while (1) {

    //
    //  If reading from stdio then print out the prompt and get
    //  comlete command line (from either stdio or ReadFile) to the
    //  newline character.  If readfile is returning empty then switch back
    //  to stdio.  Finally tokenize the command line.  This code sets up
    //  cargc and cargv similiar to argc and argv.
    //

    do {
      if (ReadFile == NULL) printf("Arbitrator > ");
      if ((fgets(Cmdline, 512, (ReadFile == NULL ? stdin: ReadFile)) == NULL) && (ReadFile != NULL)) {ReadFile = NULL; Cmdline[0] = '\0';};

      cargc = 0;
      cargv[cargc] = strtok(Cmdline, " \t\n"); 
      while (cargv[cargc] != NULL) {
        cargv[++cargc] = strtok(NULL, " \t\n");
      }
    } while (cargc == 0);

    //
    //  Now switch on the first token
    //

    if      (!strcasecmp(cargv[0], "quit"))    break;
    else if (!strcasecmp(cargv[0], "status"))  CmdStatus(cargc, cargv);
    else if (!strcasecmp(cargv[0], "log"))     CmdLog(cargc, cargv);
    else if (!strcasecmp(cargv[0], "verbose")) CmdVerbose(cargc, cargv);
    else if (!strcasecmp(cargv[0], "?"))       CmdHelp(cargc, cargv);
    else if (!strcasecmp(cargv[0], "help"))    CmdHelp(cargc, cargv);
    else if (!strcasecmp(cargv[0], "start"))   CmdStartStop(cargc, cargv, start_service);
    else if (!strcasecmp(cargv[0], "stop"))    CmdStartStop(cargc, cargv, stop_service);
    else if (!strcasecmp(cargv[0], "add"))     CmdAddRemove(cargc, cargv, add_node);
    else if (!strcasecmp(cargv[0], "remove"))  CmdAddRemove(cargc, cargv, remove_node);
    else if (!strcasecmp(cargv[0], "set"))     CmdSet(cargc, cargv);
    else if (!strcasecmp(cargv[0], "read"))    CmdRead(cargc, cargv);
    else                                       CmdUnknown(cargc, cargv);
  }

  //
  //  clean up and program and exit
  //
  // FIXME: must cleanup threads
  //        return is probably better. for some reason, probably related
  //        threads, return isn't working right now...
  printf("Cleanup, exiting arbitrator...\n");
  exit(0);
  //return 0;
}

int
CmdStatus(int cargc, char *cargv[])
{
  printf("Cmd Status\n");
  return;
}

int
CmdLog(int cargc, char *cargv[]) 
{
  printf("Cmd Log\n");
  return;
}

int
CmdVerbose(int cargc, char *cargv[]) 
{
  //
  //  The syntax is verbose [on | off].  If the arg count is great than
  //  one then just check the second parameter to see what we should.
  //

  if (cargc > 1) {
    if (!strcasecmp(cargv[1], "on"))  {
      Verbose = 1;
      printf("Verbose mode is now on\n");
      set_logging(stdout, DEBUG);
      return;
    }
    if (!strcasecmp(cargv[1], "off")) {
      Verbose = 0;
      printf("Verbose mode is now off\n");
      set_logging(stdout, INFO);
      return;
    }
  }

  //
  //  If we get here then the second parameter was not on or off, but so
  //  we will just print out what our verbose state is and return to our
  //  caller
  //

  if (Verbose) {
    printf("Verbose mode is currently on.\n");
  } else {
    printf("Verbose mode is currently off.\n");
  }
  return;
}

int
CmdStartStop(int cargc, char *cargv[], start_stop_service_routine func)
{
  service_e service;

  //
  //  if the user typed only "start" or "stop" then tell what can be done
  //

  if (cargc == 1) {
    printf("****list start stop options****\n");
    return;
  }

  //
  // see if the user wants to work on a service
  //

  if (!strcasecmp(cargv[1], "service")) {

    //
    //  if the arg count is not four then list the current services
    //

    if (cargc != 4) {
      printf("****list current services****\n");
      show_services();
      return;
    }

    //
    //  At this point the user wants to start or stop a service
    //  determine the type of service and call the supplied function
    //

    if      (!strcasecmp(cargv[2], "FTP"))   { service = FTP; }
    else if (!strcasecmp(cargv[2], "HTTP"))  { service = HTTP; }
    else if (!strcasecmp(cargv[2], "HTTPS")) { service = HTTPS; }
    else {
      printf("unknown service type \"%s\"\n", cargv[2]);
      return;
    }

    func(service, atoi(cargv[3]));

  } else {

    printf("Cannot start or stop \"%s\"\n", cargv[1]);
  }
  return;
}

int
CmdAddRemove(int cargc, char *cargv[], add_remove_node_routine func)
{
  //
  //  if the user typed only "add" or "remove" then what's possible
  //

  if (cargc == 1) {
    printf("****list options for add remove****\n");
    return;
  }

  //
  //  see if the user wants to work on a "node"
  //

  if (!strcasecmp(cargv[1], "node")) {

    //
    //  if the arg count is other than three than tell the user
    //  list of current nodes
    //

    if (cargc != 3) {
      printf("****list current set of nodes****\n");
      show_nodes();
      return;
    }

    //
    //  At this point the user wants to work on a node, so simply 
    //  call supplied routine with the supplied address
    //

    func(cargv[2]);

  } else {

    //
    //  The user tried to add/remove something unknown
    //

    printf("cannot add or remove \"%s\"\n", cargv[1]);
  }

  return;
}

int
CmdSet(int cargc, char * cargv[])
{
  //
  //  if the user typed only "set" then tell what can be set
  //

  if (cargc == 1) {
    printf("****list set options****\n");
    return;
  }

  //
  //  see if the user wants to set "algorithm"
  //

  if (!strcasecmp(cargv[1], "algorithm")) {

    //
    //  if the arg count is other than three then 
    //  tell the user the currently set algorithm
    //

    if (cargc != 3) {

      printf("Currently set algorithm is:");

      switch (current_algorithm.name) {
      case DYNAMICRATIO:
        printf(" DYNAMICRATIO\n");
        break;
      case LEASTCONNECTIONS:
        printf(" LEASTCONNECTIONS\n");
        break;
      case RATIO:
        printf(" RATIO\n");
        break;
      case ROUNDROBIN:
        printf(" ROUNDROBIN\n");
        break;
      default:
        printf(" undefined\n");
        break;
      }
      return;
    }

    //
    //  At this point the user wants to set a new algorithm
    //  find out the type asked for and set it
    //

    if      (!strcasecmp(cargv[2], "DYNAMICRATIO"))     { set_algorithm(DYNAMICRATIO); }
    else if (!strcasecmp(cargv[2], "LEASTCONNECTIONS")) { set_algorithm(LEASTCONNECTIONS); }
    else if (!strcasecmp(cargv[2], "RATIO"))            { set_algorithm(RATIO); }
    else if (!strcasecmp(cargv[2], "ROUNDROBIN"))       { set_algorithm(ROUNDROBIN); }
    else {
      printf("unknown algorithm type \"%s\"\n", cargv[2]);
    }

  } else {

    //
    // for now only algorithm can be set, all others are illegal.
    //

    printf("cannot set \"%s\"\n", cargv[1]);
  }

  return;
}

int
CmdRead(int cargc, char *cargv[])
{
  if (cargc < 2) {
    printf("must specify a file to open.\n");
    return;
  }

  if ((ReadFile = fopen(cargv[1], "r")) == NULL) {
    printf("Error in opening \"%s\"\n", cargv[1]);
  }

  return;
}

int
CmdHelp(int cargc, char *cargv[])
{
  printf("Commands are:\n");
  printf("  \"quit\" - quit this program\n");
  printf("  \"status\" - print the current system status\n");
  printf("  \"log\" - print the message log\n");
  printf("  \"verbose [on|off]\" - switch between verbose and quite mode\n");
  printf("  \"start service [FTP | HTTP | HTTPS] <port>\" \n");
  printf("  \"stop service [FTP | HTTP | HTTPS] <port>\" \n");
  printf("  \"add node <ipaddress>\" \n");
  printf("  \"remove node <ipaddress>\" \n");
  printf("  \"set algorithm [DYNAMICRATIO | LEASTCONNECTIONS | RATIO | ROUNDROBIN]\" \n");
  printf("  \"reed <file>\" - read commands from the specified file\n");
  printf("  \"help\" or \"?\" - print this help message\n");
  return;
}

int
CmdUnknown(int cargc, char *cargv[])
{
  printf("UNKNOWN COMMAND '%s'!\n", cargv[0]);
}
