/* File: C:/sam_work/git/digilent/SDSoC/work6/videodemo/SDRelease/_sds/p0/.cf_work/devreg.c */
#include "cf_lib.h"
#include "cf_request.h"
#include "devreg.h"

#include "stdio.h"  // for getting printf
#include "xlnk_core_cf.h"
#include "accel_info.h"

void _p0_cf_register(int first)
{
  int xlnk_init_done = cf_xlnk_open(first);
  if (xlnk_init_done == 0) {
    cf_xlnk_init(first);
  }
  else if (xlnk_init_done <0) {
    fprintf(stderr, "ERROR: unable to open xlnk %d\n", xlnk_init_done);
  }
  else {
  }
}

void _p0_cf_unregister(int last)
{
  xlnkClose(last,NULL);
}

