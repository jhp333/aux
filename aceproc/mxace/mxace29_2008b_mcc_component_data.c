/*
 * MATLAB Compiler: 4.9 (R2008b)
 * Date: Thu Jan 05 12:37:25 2012
 * Arguments: "-B" "macro_default" "-W" "lib:mxace29_2008b" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "elimcrcr" "addmlines" 
 */

#include "mclmcrrt.h"

#ifdef __cplusplus
extern "C" {
#endif
const unsigned char __MCC_mxace29_2008b_session_key[] = {
    '0', 'B', 'A', 'D', '7', '2', '2', '7', '1', '8', '7', '6', '2', '4', 'B',
    '5', '0', 'F', '0', '4', '9', '8', '9', 'A', '1', '8', '1', '2', '2', '6',
    '9', '9', '0', 'A', '3', '4', '8', 'F', 'B', '5', 'A', '6', 'C', '0', 'E',
    '2', '0', 'D', '0', 'F', 'A', '7', '4', '0', '8', '0', '1', 'A', 'F', '0',
    'C', 'C', 'B', '2', '0', '3', 'F', '1', '6', 'C', 'C', 'A', 'C', 'F', 'B',
    '9', 'D', 'E', '5', 'F', '3', 'D', '1', '6', 'B', 'F', 'F', '8', 'C', 'E',
    '1', '4', 'B', 'F', 'A', '7', 'B', '1', '2', 'F', '5', 'B', '1', '5', 'A',
    '0', '9', '0', '0', 'C', 'E', 'F', '8', 'F', 'C', '9', '8', '1', 'C', '7',
    'F', '2', '3', 'F', 'F', '0', '6', 'B', '6', 'E', '6', '1', 'C', 'B', '4',
    '0', 'D', '8', 'F', 'D', '2', 'C', '8', '5', '8', '3', '1', 'A', '4', '4',
    '4', 'E', '7', 'A', 'C', '8', '1', 'E', '4', '6', '0', 'B', '1', '1', 'F',
    'D', '7', '3', '2', 'A', '7', 'D', '3', '4', 'C', 'D', '9', '4', 'B', '4',
    '6', '1', '2', '5', 'E', '0', 'A', 'E', '4', 'D', 'A', 'B', 'D', 'A', '0',
    '5', 'B', '4', 'D', 'C', '3', 'C', '6', 'C', '9', '6', 'D', '1', 'A', 'D',
    '0', 'F', 'B', 'A', 'A', '7', 'C', '2', '0', '9', '3', '4', 'F', 'A', 'F',
    '3', '9', '8', '3', 'F', 'B', '5', 'A', '1', '1', '6', '3', '1', 'B', '5',
    'A', 'D', '9', '3', '0', 'D', 'E', '8', 'D', '1', 'D', 'B', '3', 'A', '7',
    '0', '\0'};

const unsigned char __MCC_mxace29_2008b_public_key[] = {
    '3', '0', '8', '1', '9', 'D', '3', '0', '0', 'D', '0', '6', '0', '9', '2',
    'A', '8', '6', '4', '8', '8', '6', 'F', '7', '0', 'D', '0', '1', '0', '1',
    '0', '1', '0', '5', '0', '0', '0', '3', '8', '1', '8', 'B', '0', '0', '3',
    '0', '8', '1', '8', '7', '0', '2', '8', '1', '8', '1', '0', '0', 'C', '4',
    '9', 'C', 'A', 'C', '3', '4', 'E', 'D', '1', '3', 'A', '5', '2', '0', '6',
    '5', '8', 'F', '6', 'F', '8', 'E', '0', '1', '3', '8', 'C', '4', '3', '1',
    '5', 'B', '4', '3', '1', '5', '2', '7', '7', 'E', 'D', '3', 'F', '7', 'D',
    'A', 'E', '5', '3', '0', '9', '9', 'D', 'B', '0', '8', 'E', 'E', '5', '8',
    '9', 'F', '8', '0', '4', 'D', '4', 'B', '9', '8', '1', '3', '2', '6', 'A',
    '5', '2', 'C', 'C', 'E', '4', '3', '8', '2', 'E', '9', 'F', '2', 'B', '4',
    'D', '0', '8', '5', 'E', 'B', '9', '5', '0', 'C', '7', 'A', 'B', '1', '2',
    'E', 'D', 'E', '2', 'D', '4', '1', '2', '9', '7', '8', '2', '0', 'E', '6',
    '3', '7', '7', 'A', '5', 'F', 'E', 'B', '5', '6', '8', '9', 'D', '4', 'E',
    '6', '0', '3', '2', 'F', '6', '0', 'C', '4', '3', '0', '7', '4', 'A', '0',
    '4', 'C', '2', '6', 'A', 'B', '7', '2', 'F', '5', '4', 'B', '5', '1', 'B',
    'B', '4', '6', '0', '5', '7', '8', '7', '8', '5', 'B', '1', '9', '9', '0',
    '1', '4', '3', '1', '4', 'A', '6', '5', 'F', '0', '9', '0', 'B', '6', '1',
    'F', 'C', '2', '0', '1', '6', '9', '4', '5', '3', 'B', '5', '8', 'F', 'C',
    '8', 'B', 'A', '4', '3', 'E', '6', '7', '7', '6', 'E', 'B', '7', 'E', 'C',
    'D', '3', '1', '7', '8', 'B', '5', '6', 'A', 'B', '0', 'F', 'A', '0', '6',
    'D', 'D', '6', '4', '9', '6', '7', 'C', 'B', '1', '4', '9', 'E', '5', '0',
    '2', '0', '1', '1', '1', '\0'};

static const char * MCC_mxace29_2008b_matlabpath_data[] = 
  { "mxace29_2008/", "$TOOLBOXDEPLOYDIR/",
    "Users/bkwon/dev/vc8/aceproc/matlab/",
    "Users/bkwon/matlab/work/", "nic/4.20/MATLAB/FTM/",
    "nic/4.20/MATLAB/Filterbank/", "nic/4.20/MATLAB/FrontEnd/",
    "nic/4.20/MATLAB/Implant/", "nic/4.20/MATLAB/LoudnessGrowth/",
    "nic/4.20/MATLAB/Processing/", "nic/4.20/MATLAB/Sequence/",
    "nic/4.20/MATLAB/Strategy/", "nic/4.20/MATLAB/Utility/",
    "$TOOLBOXMATLABDIR/general/", "$TOOLBOXMATLABDIR/ops/",
    "$TOOLBOXMATLABDIR/lang/", "$TOOLBOXMATLABDIR/elmat/",
    "$TOOLBOXMATLABDIR/randfun/", "$TOOLBOXMATLABDIR/elfun/",
    "$TOOLBOXMATLABDIR/specfun/", "$TOOLBOXMATLABDIR/matfun/",
    "$TOOLBOXMATLABDIR/datafun/", "$TOOLBOXMATLABDIR/polyfun/",
    "$TOOLBOXMATLABDIR/funfun/", "$TOOLBOXMATLABDIR/sparfun/",
    "$TOOLBOXMATLABDIR/scribe/", "$TOOLBOXMATLABDIR/graph2d/",
    "$TOOLBOXMATLABDIR/graph3d/", "$TOOLBOXMATLABDIR/specgraph/",
    "$TOOLBOXMATLABDIR/graphics/", "$TOOLBOXMATLABDIR/uitools/",
    "$TOOLBOXMATLABDIR/strfun/", "$TOOLBOXMATLABDIR/imagesci/",
    "$TOOLBOXMATLABDIR/iofun/", "$TOOLBOXMATLABDIR/audiovideo/",
    "$TOOLBOXMATLABDIR/timefun/", "$TOOLBOXMATLABDIR/datatypes/",
    "$TOOLBOXMATLABDIR/verctrl/", "$TOOLBOXMATLABDIR/codetools/",
    "$TOOLBOXMATLABDIR/helptools/", "$TOOLBOXMATLABDIR/winfun/",
    "$TOOLBOXMATLABDIR/demos/", "$TOOLBOXMATLABDIR/timeseries/",
    "$TOOLBOXMATLABDIR/hds/", "$TOOLBOXMATLABDIR/guide/",
    "$TOOLBOXMATLABDIR/plottools/", "toolbox/local/",
    "toolbox/shared/dastudio/", "$TOOLBOXMATLABDIR/datamanager/",
    "toolbox/compiler/", "toolbox/signal/signal/",
    "toolbox/signal/sigtools/", "toolbox/shared/spcuilib/" };

static const char * MCC_mxace29_2008b_classpath_data[] = 
  { "" };

static const char * MCC_mxace29_2008b_libpath_data[] = 
  { "" };

static const char * MCC_mxace29_2008b_app_opts_data[] = 
  { "" };

static const char * MCC_mxace29_2008b_run_opts_data[] = 
  { "" };

static const char * MCC_mxace29_2008b_warning_state_data[] = 
  { "off:MATLAB:dispatcher:nameConflict" };


mclComponentData __MCC_mxace29_2008b_component_data = { 

  /* Public key data */
  __MCC_mxace29_2008b_public_key,

  /* Component name */
  "mxace29_2008b",

  /* Component Root */
  "",

  /* Application key data */
  __MCC_mxace29_2008b_session_key,

  /* Component's MATLAB Path */
  MCC_mxace29_2008b_matlabpath_data,

  /* Number of directories in the MATLAB Path */
  53,

  /* Component's Java class path */
  MCC_mxace29_2008b_classpath_data,
  /* Number of directories in the Java class path */
  0,

  /* Component's load library path (for extra shared libraries) */
  MCC_mxace29_2008b_libpath_data,
  /* Number of directories in the load library path */
  0,

  /* MCR instance-specific runtime options */
  MCC_mxace29_2008b_app_opts_data,
  /* Number of MCR instance-specific runtime options */
  0,

  /* MCR global runtime options */
  MCC_mxace29_2008b_run_opts_data,
  /* Number of MCR global runtime options */
  0,
  
  /* Component preferences directory */
  "mxace29_2008_484ED8EF81417AC363E4356B7467EEB9",

  /* MCR warning status data */
  MCC_mxace29_2008b_warning_state_data,
  /* Number of MCR warning status modifiers */
  1,

  /* Path to component - evaluated at runtime */
  NULL

};

#ifdef __cplusplus
}
#endif


