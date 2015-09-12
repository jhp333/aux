/*
 * MATLAB Compiler: 4.9 (R2008b)
 * Date: Fri Jun 15 23:55:31 2012
 * Arguments: "-B" "macro_default" "-W" "lib:mxace32_2008b_" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "turn2cic4" "show_p" 
 */

#include "mclmcrrt.h"

#ifdef __cplusplus
extern "C" {
#endif
const unsigned char __MCC_mxace32_2008b__session_key[] = {
    '0', 'D', '2', '4', '2', 'C', '7', '4', 'C', '8', 'C', '5', '2', '6', '3',
    '4', '0', 'D', 'C', 'E', 'D', '6', '5', '3', '7', '1', 'B', '4', '6', '3',
    '6', '5', '0', '6', '4', 'E', '6', '3', 'D', 'C', '7', '5', '4', '9', 'A',
    'B', '1', 'C', '5', '7', '0', '9', 'E', '9', '8', 'A', '2', '8', '9', 'C',
    '2', '9', '6', 'E', 'F', '1', '4', '9', '7', '2', '3', '6', '6', '0', '2',
    'E', '6', '8', 'F', '2', '9', '0', '9', '4', '4', '6', 'A', '4', 'A', '5',
    '1', 'A', '5', 'E', '2', '0', '5', 'E', '9', 'B', 'F', 'C', 'A', 'B', '1',
    '6', '7', 'F', '5', '3', '4', '1', '5', '7', '0', '3', '5', 'D', 'B', 'D',
    'A', '3', 'B', '6', '6', '6', 'B', 'D', 'B', '6', 'D', '1', 'F', '7', '4',
    '1', '2', 'F', '5', 'C', 'A', 'D', 'B', '4', '6', 'C', '3', '9', '5', '4',
    '1', 'D', 'C', '1', '9', 'A', '5', 'F', '6', 'E', '0', 'E', 'A', '6', '6',
    'A', '4', '2', '3', 'F', '3', '2', '6', '7', 'D', '7', '8', 'F', '8', '3',
    '5', 'E', 'E', 'F', 'A', '1', '8', '3', '1', 'D', 'F', 'A', 'A', '5', '6',
    '1', '6', '1', '2', '0', 'B', 'C', '2', 'F', '2', '6', 'A', 'C', '2', 'B',
    '9', '8', '4', 'E', '5', '7', '4', 'B', '4', '6', '1', '0', '6', '1', '0',
    '8', 'B', '1', 'B', '1', 'C', '0', '6', '2', 'A', 'C', '4', '4', 'D', '2',
    'E', '5', '2', 'B', '5', '1', 'A', 'C', 'A', '6', '8', '3', '9', '4', '5',
    'C', '\0'};

const unsigned char __MCC_mxace32_2008b__public_key[] = {
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

static const char * MCC_mxace32_2008b__matlabpath_data[] = 
  { "mxace32_2008/", "$TOOLBOXDEPLOYDIR/", "dev/VC8/aceproc/mxace/",
    "MATLAB/work/", "nic/2/matlab/m_files/", "nic/2/binaries/",
    "nic/4.20/MATLAB/FTM/", "nic/4.20/MATLAB/Filterbank/",
    "nic/4.20/MATLAB/FrontEnd/", "nic/4.20/MATLAB/Implant/",
    "nic/4.20/MATLAB/LoudnessGrowth/",
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

static const char * MCC_mxace32_2008b__classpath_data[] = 
  { "" };

static const char * MCC_mxace32_2008b__libpath_data[] = 
  { "" };

static const char * MCC_mxace32_2008b__app_opts_data[] = 
  { "" };

static const char * MCC_mxace32_2008b__run_opts_data[] = 
  { "" };

static const char * MCC_mxace32_2008b__warning_state_data[] = 
  { "off:MATLAB:dispatcher:nameConflict" };


mclComponentData __MCC_mxace32_2008b__component_data = { 

  /* Public key data */
  __MCC_mxace32_2008b__public_key,

  /* Component name */
  "mxace32_2008b_",

  /* Component Root */
  "",

  /* Application key data */
  __MCC_mxace32_2008b__session_key,

  /* Component's MATLAB Path */
  MCC_mxace32_2008b__matlabpath_data,

  /* Number of directories in the MATLAB Path */
  55,

  /* Component's Java class path */
  MCC_mxace32_2008b__classpath_data,
  /* Number of directories in the Java class path */
  0,

  /* Component's load library path (for extra shared libraries) */
  MCC_mxace32_2008b__libpath_data,
  /* Number of directories in the load library path */
  0,

  /* MCR instance-specific runtime options */
  MCC_mxace32_2008b__app_opts_data,
  /* Number of MCR instance-specific runtime options */
  0,

  /* MCR global runtime options */
  MCC_mxace32_2008b__run_opts_data,
  /* Number of MCR global runtime options */
  0,
  
  /* Component preferences directory */
  "mxace32_2008_B0DC3B4C23C35AB9BE1A85241E87A8EB",

  /* MCR warning status data */
  MCC_mxace32_2008b__warning_state_data,
  /* Number of MCR warning status modifiers */
  1,

  /* Path to component - evaluated at runtime */
  NULL

};

#ifdef __cplusplus
}
#endif


