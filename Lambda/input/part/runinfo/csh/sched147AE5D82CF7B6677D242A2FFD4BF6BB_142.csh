#!/bin/csh
# -------------------------------------- 
# Script generated on Tue Nov 20 16:49:32 EST 2018 by the STAR Unified Meta Scheduler 1.10.28 and submitted with:
# cd /star/u/jhai/STAR/FixedTarget/H3LAndH4L-FXT/Lambda/input/part; condor_submit /star/u/jhai/STAR/FixedTarget/H3LAndH4L-FXT/Lambda/input/part/runinfo/csh/sched147AE5D82CF7B6677D242A2FFD4BF6BB_82_181.condor
# --------------------------------------



if ( $?SUMS_EXECUTION ) then
    goto USERCODESECTION
endif





/bin/echo 'We are starting on node: '`/bin/hostname`


# Preparing environment variables
ENVSETUPSECTION:

setenv EVENTS_START "-1"
setenv EVENTS_STOP "-1"
setenv FILEBASENAME "st_physics_16140038_raw_1000005"
setenv FILELIST "/star/u/jhai/STAR/FixedTarget/H3LAndH4L-FXT/Lambda/input/part/runinfo/list/sched147AE5D82CF7B6677D242A2FFD4BF6BB_142.list"
setenv FILELIST_ALL "/star/u/jhai/STAR/FixedTarget/H3LAndH4L-FXT/Lambda/input/part/runinfo/csh/sched147AE5D82CF7B6677D242A2FFD4BF6BB.list"
setenv INPUTFILECOUNT "1"
setenv JOBID "147AE5D82CF7B6677D242A2FFD4BF6BB_142"
setenv JOBINDEX "142"
setenv LOGGING "STD"
setenv REQUESTID "147AE5D82CF7B6677D242A2FFD4BF6BB"
setenv SUBMITATTEMPT "1"
setenv SUBMITTINGDIRECTORY "/star/u/jhai/STAR/FixedTarget/H3LAndH4L-FXT/Lambda/input/part"
setenv SUBMITTINGNODE "rcas6009.rcf.bnl.gov"
setenv SUBMIT_TIME "2018-11-20 21:49:32"
setenv SUMS_AUTHENTICATED_USER "jhai@rhic.bnl.gov"
setenv SUMS_USER "jhai"
setenv SUMS_nProcesses "282"
setenv SUMS_name "null"
setenv USEXROOTD "1"
setenv INPUTFILE0 "root://xrdstar.rcf.bnl.gov:1095//home/starreco/reco/fixedTarget2015/ReversedFullField/P16ia/2015/140/16140038/st_physics_16140038_raw_1000005.MuDst.root"

# This big blobk is used by CONDOR where $HOME in not set

if ( ! $?USER ) then
    echo "USER is not defined"
    set USER=`id | sed "s/).*//" | sed "s/.*(//"`
endif
if ( ! $?HOME ) then
    echo "HOME is not defined"

    if ( -x /usr/bin/getent ) then
	# we have getent, should not be on aix, bsd, Tru64 however
	# will work for Linux
	echo "Using getent method"
	setenv HOME `/usr/bin/getent passwd $USER | /bin/sed 's|.*\:.*\:.*\:.*\:\([^\:]*\):.*|\1|'`
    else 
	set PTEST=`which perl`
	if ( "$PTEST" != "" ) then
	    echo "Using perl method"
	    # we have perl defined, we can get info from there
	    /bin/cat <<EOF >test$$.pl
my(\$user) = getpwuid(\$<);
@items = getpwnam(\$user);
print \$items[7];
EOF
	    setenv HOME `$PTEST test$$.pl` && /bin/rm  -f test$$.pl
	else
	    set CTEST=`which cc`
	    if ( "$CTEST" != "" ) then
		echo "Using C code method"
		# use C code for doing this
		/bin/cat <<EOF >test$$.c
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>


int main()
{
  struct passwd *info;
  uid_t          uid;

  /* get process UID */
  uid = getuid();
  info= getpwuid(uid);

  (void) printf("%s\n",info->pw_dir);
  return 0;
} 

EOF
		$CTEST -o test$$ test$$.c
		/bin/chmod +x test$$
		setenv HOME `./test$$` && /bin/rm -f test$$ test$$.c
	    else
		echo "We have no ways to define HOME and it is not defined"
		exit
	    endif
	endif
    endif
endif

echo "HOME is now $HOME"


# Default value for path if not defined.
if ( ! $?PATH ) then
   setenv PATH /usr/local/bin:/bin:/usr/bin
endif


/usr/bin/test -r $HOME/.cshrc && source $HOME/.cshrc

# Creating the scratch directory, return failure status
set SUMS_try=0
set SUMS_PAD=""

MKDTRY:
setenv SCRATCH "/tmp/$USER$SUMS_PAD/$JOBID"
/bin/mkdir -p $SCRATCH   >& /dev/null
set STS=$status
if (! -d $SCRATCH) then
       #test if porper UID
       set SUMS_IsUID=`(/usr/bin/test -O /tmp/$USER$SUMS_PAD && echo 1) || echo 0`
       if ( $SUMS_try == 0 &&  -e "/tmp/$USER$SUMS_PAD" && ! $SUMS_IsUID) then
            # First try, directory exists but is not owned by $USER
            # Create a different path and try again
            echo "Scheduler:: $SCRATCH not owned by $USER, trying alternative"
            @ try++
            set SUMS_seed=`/bin/date +%j%H%M%S`
            set SUMS_PAD=`expr $SUMS_seed  \* 25973 \% 100`
            goto MKDTRY
        else
		 echo "Scheduler:: Failed to create $SCRATCH on $HOST"
	         exit $STS
        endif

endif


##########################################################
# Detached script pid so that signals will propagate to  #
# child processes such as root4star, giving a chance to  #
# close out open files.                                  #
##########################################################
if ( ! $?SUMS_EXECUTION ) then
    echo "$$ We will spawn ourselves ($0 $*)"
    echo "$$ We BEGIN on [`/bin/date`]"

    onintr SUMS_SIGHANDLER
    ((setenv SUMS_EXECUTION 1 && nohup $0 $*) > $SCRATCH/.output.log ) >& $SCRATCH/.errror.log & 
    set PID=$!

    echo "$$ We are waiting for $PID "
    set loop=0
    set DELAY=1
    set CHLDPID=""
    set CHKP=10
    while (`/bin/ps --no-headers -p $PID` != "")
       if ( $loop % $CHKP == 0 ) then
           # check this only once every CHKP*DELAY seconds
           set GTEST=`/bin/ps -efH | /bin/grep $PID | /bin/grep -v $$ |  /bin/grep -v grep | /usr/bin/head -1 | /usr/bin/awk '{print $2}'`
           if ( "$GTEST" != "" && "$GTEST" != "$CHLDPID") then
               echo "$$ Grandchild PID is $GTEST - found in $loop x $DELAY seconds"
               if ( "$CHLDPID" == "") then
                   echo "$$ Establishing first watcher   on    `/bin/date`"
                   set DELAY=2
               else
                   echo "$$ Grandchild to watch has changed on `/bin/date`"
               endif
               set CHLDPID="$GTEST"
               echo "$$ Process tree is $$ (main) -> $PID (child) -> $CHLDPID (grandchild)"
               /bin/ps -l $CHLDPID
            endif
       endif
       @ loop++
       sleep $DELAY
   end
   cat $SCRATCH/.output.log >> /dev/stdout
    cat $SCRATCH/.errror.log >> /dev/stderr

# Delete the scratch directory
    /bin/rm -fr $SCRATCH
    echo "$$ Execution of ($0 $*) has ended, exiting normally"
    echo "$$ We END on [`/bin/date`]"
    exit 0

    SUMS_SIGHANDLER:
        echo "$$ We received a signal - sending TERM to PID=$PID group"
        kill -TERM -$PID
        echo "$$ We have sent an TERM signal to $PID and waiting for $CHLDPID on `/bin/date`"
        while (`/bin/ps --no-headers -p $CHLDPID` != "")
           sleep 2
        end
        echo "$$ Dumping the output/error channels ---->"
        cat $SCRATCH/.output.log >> /dev/stdout
        cat $SCRATCH/.errror.log >> /dev/stderr
        echo "$$ <---- done with output/error channels"
        echo "$$ We are now leaving with error on [`/bin/date`]"
        exit 1
endif

USERCODESECTION:
echo "$$ Stepping into the main SUMS wrapper program with [$*]"



#Note: The default directory in which jobs start has been fix to $SCRATCH
cd $SCRATCH


echo "--------------------------------"
echo 'STAR Unified Meta Scheduler 1.10.28 we are starting in $SCRATCH :' `/bin/pwd`
echo "--------------------------------"
/bin/ls -l

###################################################
# User command BEGIN ----------------------------->

		set mainDir = /star/u/jhai/STAR/FixedTarget/H3LAndH4L-FXT/Lambda
		cp -rf ${mainDir}/.sl73_gcc485 .
		cp -rf ${mainDir}/StRoot .
		cp -f  ${mainDir}/reco.C .

		starnew

		root4star -q -b reco.C\(\"$FILELIST\",$INPUTFILECOUNT,0,\"$SCRATCH/\",\"$JOBID\"\)
	
# <------------------------------User command END
###################################################

# Copy output files (if any where specified)
    
    ########### Copy Command Block ########### 
    echo "Starting copy block"
    foreach file ( $SCRATCH/*.root )
         set i=0
         
         if ( -d $file ) then
            set cpCommand="/bin/cp -r $file /star/u/jhai/scratch/FixedTarget/Lambda/output/"
         else
            set cpCommand="/bin/cp -fp $file /star/u/jhai/scratch/FixedTarget/Lambda/output/"
         endif
         
         RETRY_CP:
         echo "Running copy command: " $cpCommand
         $cpCommand
         if ( $status && $i < 15 ) then
             echo "The copy failed, waiting to try again."
             @ i++
             sleep 60
             echo "Trying copy again at "`/bin/date`
             goto RETRY_CP
         endif

    end
    ##########################################
            
