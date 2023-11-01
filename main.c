#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "c_canned.h"
#include "c_string.h"


void exitIfError(int iErr, t_can* pcanMan) {
  if (iErr == C_CANNED_ERR) {
    perror(pcanMan->csErr.cStr);
    exit(pcanMan->iErr);
  }
}

void writeTest(void) {
  t_can canMan = {0};

  exitIfError(canNew(&canMan, "vcan0"), &canMan);

  printf("%s at index %d\n", canMan.csIfName.cStr, canMan.ifRequest.ifr_ifindex);

  // Set manually
  canMan.canFrame.can_id  = 0x123;
  canMan.canFrame.can_dlc = 5;
  canMan.canFrame.data[0] = 0x01;
  canMan.canFrame.data[1] = 0x23;
  canMan.canFrame.data[2] = 0x45;
  canMan.canFrame.data[3] = 0x67;
  canMan.canFrame.data[4] = 0x89;

  exitIfError(canWrite(&canMan), &canMan);

  canFree(&canMan);
}

void readTest(void) {
  t_can canMan = {0};

  exitIfError(canNew(&canMan, "vcan0"), &canMan);

  while(1) {
    exitIfError(canRead(&canMan), &canMan);

    printf("0x%03X [%d] ",canMan.canFrame.can_id, canMan.canFrame.can_dlc);

    for (int i = 0; i < canMan.canFrame.can_dlc; ++i)
      printf("%02x ",canMan.canFrame.data[i]);

    printf("\n");
  }

  canFree(&canMan);
}


int main(void) {
  writeTest();
  // readTest();

  return 0;
}
