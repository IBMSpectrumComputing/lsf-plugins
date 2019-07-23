# NFS Client Operations for LSF

## Description

This simple ELIM is meant to be an example as to how to track NFS client operations in LSF and LSF RTM for various uses as described below.  

## Use Cases

These NFS operations can be used for the following purposes:

1) Reporting in tools like RTM using ELIM Templates

2) Sorting of devices to be used for dispatch.  For example:


	bsub -R "order[nfsops]" ./a.out


Which would dispatch to the host with the least NFS operations per interval.

3) Scheduling thresholds in lsb.hosts

To do this, you would add the nfsops to the lsb.hosts 'Begin Hosts' section and then add a schedule stop and suspend thresholds to either stop dispatch of new jobs, or to start suspending jobs until the NFS operations go below the setting.


	Begin Host
	HOST_NAME MXJ   nfsops    r1m     pg    ls     tmp  DISPATCH_WINDOW  AFFINITY
	default   !     2000/8000 ()      ()    ()     ()   ()               (Y)
	End Host


4) To possibly be used as a feeder elim to be used by the LSF Master to potentially reserve NFS operations at the cluster level.  To do this, you would write a shared resource that would show the theoretical limit of NFS operations for the entire cluster as the max value, and then subtract from that, the total NFS operations in use from all compute host.  This number could then be additionally reduced via LSF job reservations with a duration.  For example, lets say your storage vendor has stated that their storage system would sustain 8M IOPs, that would be your ceiling, from there, your shared resource would essentially run the following:


	lsload -o "nfsops"


And sum that output across the cluster, and deduct that number from the total of 8M or course converting the nfsops to a rate beforehand, in the default, it would be to divide the output from the command above by 10 (the sleep interval in the elim).  This would essentially report the available IOPs on the storage.

Assuming that that number would be reported as a shared resource called totnfsops, you could then submit a job as follows:


	bsub -R "rusage[totnfsops=1000:duration=60]" ./a.out


This job would reserve 1000 NFS operations per 10 seconds for a period of 60 seconds, which would allow the job the time to start generating I/O, at that point, the reservation would expire.

## Installation Instructions

To install this elim, you must first add the "nfsops" numeric resource to LSF as per the normal process which involves updating your lsf.shared and lsf.cluster files to include the value.  Ensure that you assign this resource to hosts that you wish to clear user processes from automatically.

Then, before restarting the cluster, make sure that the elim.nfsops binary has been copied to the $LSF_SERVERDIR for all compute hosts and marked executable.  After which, you can restart your cluster using:


	lsadmin reconfig
	badmin reconfig


Make sure you restart all LIM's and not just the Master LIM.  From each compute host, you should then see the binary running in the background.  If not, you should debug the binary interactively using simply by running from the command line on your hosts.

You can also get the status of the number of NFS operations taking place on your compute
hosts by running the following lsload command:


	lsload -o "nfsops"


## Community Contribution Requirements

Community contributions to this repository must follow the [IBM Developer's Certificate of Origin (DCO)](https://github.com/IBMSpectrumComputing/platform-python-lsf-api/blob/master/IBMDCO.md) process and only through GitHub Pull Requests:

 1. Contributor proposes new code to community.

 2. Contributor signs off on contributions 
    (i.e. attachs the DCO to ensure contributor is either the code 
    originator or has rights to publish. The template of the DCO is included in
    this package).
 
 3. IBM Spectrum LSF development reviews contribution to check for:
    i)  Applicability and relevancy of functional content 
    ii) Any obvious issues

 4. If accepted, posts contribution. If rejected, work goes back to contributor and is not merged.

## Copyright

(C) Copyright IBM Corporation 2016-2019

U.S. Government Users Restricted Rights - Use, duplication or disclosure 
restricted by GSA ADP Schedule Contract with IBM Corp.

IBM(R), the IBM logo and ibm.com(R) are trademarks of International Business Machines Corp., 
registered in many jurisdictions worldwide. Other product and service names might be trademarks 
of IBM or other companies. A current list of IBM trademarks is available on the Web at 
"Copyright and trademark information" at [IBM Legal](www.ibm.com/legal/copytrade.shtml).

