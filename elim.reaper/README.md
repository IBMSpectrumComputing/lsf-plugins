# Non Job User Process Reaper Service for IBM Spectrum LSF

## Description

This Non Job User Process Reaper for LSF will remove any unauthroized users from the
compute hosts once their jobs have left those system.  It provides an optional grace period called the LSF_LINGER_TIME that you can set in the lsf.reaper file in your $LSF_ENVDIR.  The value is in seconds.

You can optionally set a LSF_SLEEP_TIME to a value in seconds as well.  As you clusters become larger, you may not want them querying the batch system as much, so increasing this value may be practical in such cases.  This value is also in seconds.

## How to Build and Install the Module

1. (Optional) Edit Makefile and update compiler or linker options
2. Source your LSF environment using either (profile.lsf or cshrc.lsf)
3. Run `make clean;make`
4. Run `make install` or copy elim.reaper to ${LSF_SERVERDIR}, and the default lsf.reaper to the ${LSF_ENVDIR}

## Installation Instructions

To install this elim, you must first add the "reaper" numeric resource to LSF as per the normal process which involves updating your lsf.shared and lsf.cluster files to include the value.  Ensure that you assign this resource to hosts that you wish to clear user processes from automatically.

Then, before restarting the cluster, make sure that the elim.reaper binary has been copied to the $LSF_SERVERDIR for all compute hosts, and that the lsf.reaper file has been copied to the $LSF_ENVDIR of your cluster or hosts.  After which, you can restart your cluster using:

`lsadmin reconfig`
`badmin reconfig`

Make sure you restart all LIM's and not just the Master LIM.  From each compute host, you should then see the binary running in the background.  If not, you should debug the binary interactively using the --debug option.

## General Usage

This Non Job User Process Reaper for LSF will remove any unauthroized users from the
compute hosts once their jobs have left the system.  It provides an optional grace period called the LSF_LINGER_TIME that you can set in the lsf.reaper file in your $LSF_ENVDIR.  The value is in seconds.

You can optionally set a LSF_SLEEP_TIME to a value in seconds as well.  As you clusters become larger, you may not want them querying the batch system as much, so increasing this value may be practical in such cases.  This value is also in seconds.

If you wish to see debug output from the elim, you may run it with the --debug option.
