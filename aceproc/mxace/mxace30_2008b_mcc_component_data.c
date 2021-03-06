/*
 * MATLAB Compiler: 4.9 (R2008b)
 * Date: Thu Feb 02 21:48:21 2012
 * Arguments: "-B" "macro_default" "-W" "lib:mxace30_2008b" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "addmlines" "turn2cic4" "show_p" 
 */

#include "mclmcrrt.h"

#ifdef __cplusplus
extern "C" {
#endif
const unsigned char __MCC_mxace30_2008b_session_key[] = {
    '5', '2', 'A', 'E', 'A', '8', 'D', 'C', 'D', 'E', 'C', '2', '1', '2', '6',
    '8', 'F', '0', 'B', 'A', '2', '5', '0', 'C', 'D', 'B', 'E', 'D', '0', 'C',
    'F', '8', '5', '6', '5', '5', '1', '4', '5', 'F', 'A', '7', '2', 'B', 'D',
    '4', '5', 'F', 'E', '9', '5', '5', 'E', 'D', 'E', '6', '0', 'E', 'D', '6',
    'A', 'C', '3', 'E', 'F', '9', '9', 'D', 'E', 'B', 'D', '1', '2', 'A', 'C',
    '6', '9', 'A', '8', '5', 'A', '7', '8', 'D', '1', '7', '4', 'D', 'B', '9',
    'A', '2', '8', '3', '5', '2', '7', 'D', 'A', '1', 'C', '4', 'F', '4', '5',
    '6', '0', '4', 'D', 'B', 'D', '8', '0', 'B', '0', '8', '7', '3', '3', '4',
    '4', 'F', '9', '6', 'D', '0', 'E', 'B', '7', 'F', 'C', 'A', 'E', 'B', '2',
    '5', '2', '7', '9', 'D', '7', 'A', '3', 'A', '7', '2', '7', '9', 'C', '5',
    '3', '5', '0', '7', 'E', '9', 'C', '7', '3', '6', '4', '7', 'A', '7', '1',
    '0', '2', 'B', '4', 'D', '9', '3', '7', '3', 'F', '4', '4', '5', '4', '5',
    '3', '9', '0', '4', '8', '6', '8', '8', '5', 'E', '5', 'F', '7', 'E', 'D',
    'B', 'F', 'E', 'F', '0', '6', '3', '0', 'C', '6', 'B', '1', '8', 'C', '8',
    '8', '6', '2', 'C', '4', '8', '1', '5', '8', 'C', '2', '1', 'E', '8', '4',
    '5', '8', '3', '2', 'B', 'E', 'F', 'B', '1', '9', 'E', '7', '7', '9', '0',
    'E', '0', '3', 'B', '7', 'A', '6', '3', '7', '7', '7', '7', '6', '9', '4',
    'A', '\0'};

const unsigned char __MCC_mxace30_2008b_public_key[] = {
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

static const char * MCC_mxace30_2008b_matlabpath_data[] = 
  { "mxace30_2008/", "$TOOLBOXDEPLOYDIR/",
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

static const char * MCC_mxace30_2008b_classpath_data[] = 
  { "" };

static const char * MCC_mxace30_2008b_libpath_data[] = 
  { "" };

static const char * MCC_mxace30_2008b_app_opts_data[] = 
  { "" };

static const char * MCC_mxace30_2008b_run_opts_data[] = 
  { "" };

static const char * MCC_mxace30_2008b_warning_state_data[] = 
  { "off:MATLAB:dispatcher:nameConflict" };


mclComponentData __MCC_mxace30_2008b_component_data = { 

  /* Public key data */
  __MCC_mxace30_2008b_public_key,

  /* Component name */
  "mxace30_2008b",

  /* Component Root */
  "",

  /* Application key data */
  __MCC_mxace30_2008b_session_key,

  /* Component's MATLAB Path */
  MCC_mxace30_2008b_matlabpath_data,

  /* Number of directories in the MATLAB Path */
  52,

  /* Component's Java class path */
  MCC_mxace30_2008b_classpath_data,
  /* Number of directories in the Java class path */
  0,

  /* Component's load library path (for extra shared libraries) */
  MCC_mxace30_2008b_libpath_data,
  /* Number of directories in the load library path */
  0,

  /* MCR instance-specific runtime options */
  MCC_mxace30_2008b_app_opts_data,
  /* Number of MCR instance-specific runtime options */
  0,

  /* MCR global runtime options */
  MCC_mxace30_2008b_run_opts_data,
  /* Number of MCR global runtime options */
  0,
  
  /* Component preferences directory */
  "mxace30_2008_F530E0406CEE036B0AD66725BE43A515",

  /* MCR warning status data */
  MCC_mxace30_2008b_warning_state_data,
  /* Number of MCR warning status modifiers */
  1,

  /* Path to component - evaluated at runtime */
  NULL

};

#ifdef __cplusplus
}
#endif


