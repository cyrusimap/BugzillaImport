#!/bin/bash

checkid() {
  if [ `id -u` -eq 0 ] ; then
    echo "quota: must run as the Cyrus user"
    exit 75
  fi
}

setconfigdirectory() {
  if [ -f $1 ] ; then
    configdirectory=`grep configdirectory $1 | cut -d: -f2`
    defaultpartition=`grep defaultpartition $1 | cut -d: -f2 | sed 's# ##g'`
    if [ x$defaultpartion = "x" ] ; then
      defaultpartition=default
    fi
    partition=`grep partition-$defaultpartition $1 | cut -d: -f2`    
  elif [ x$2x != "xinitx" ] ; then
      echo -n "quota: can't open configuration file $1: " 1>&2
      echo "No such file or directory" 1>&2
      exit 75
  fi
}

rightalign() {
  ALIGNVALUE=$1
  PRINTVALUE=$2
  if [ $PRINTVALUE -lt 1 ] ; then
    ALIGNVALUE=$[ALIGNVALUE - 1]
  else 
    while [ $PRINTVALUE -ge 1 ] ; do
      ALIGNVALUE=$[ALIGNVALUE - 1]
      PRINTVALUE=$[PRINTVALUE / 10]
    done
  fi
  while [ $ALIGNVALUE -gt 0 ] ; do
    echo -n " "
    ALIGNVALUE=$[ALIGNVALUE - 1]
  done
  echo -n $2
}

displayheader() {
  echo "   Quota  % Used    Used Root"
}

quotafile() {
  ROOTID=$1
  USERID=`echo $ROOTID | cut -d"." -f2`
  HASHCHAR=`echo $USERID | cut -c1`
  QUOTAFILE="$configdirectory/quota/$HASHCHAR/user.$USERID"
  QUOTAUSED=`head -n 1 $QUOTAFILE`
  QUOTAKB=`tail -n 1 $QUOTAFILE`
}

displayquota() {
  QUOTABYTES=`echo "$QUOTAKB * 1024" | bc`
  PERCENTUSED=`echo "$QUOTAUSED * 100 / $QUOTABYTES" | bc`
  QUOTAUSEDKB=`echo "$QUOTAUSED / 1024" | bc`
  rightalign 8 $QUOTAKB
  rightalign 8 $PERCENTUSED
  rightalign 8 $QUOTAUSEDKB
  echo " $ROOTID"
}

fixquota() {
#  DUOUT=`du --exclude="cyrus[.]*" -sb $partition/user/$USERID`
#  NEWQUOTAUSED=`echo $DUOUT | cut -d" " -f1`
  FINDOUT=`find $partition/user/$USERID \
           -type f -regex ".*/[0-9]*[.]$" -printf "+%s"`
  NEWQUOTAUSED=`echo 0$FINDOUT | bc`
  if [ $NEWQUOTAUSED -ne $QUOTAUSED ] ; then
    echo "$ROOTID: usage was $QUOTAUSED, new $NEWQUOTAUSED"
    echo -e "$NEWQUOTAUSED\n$QUOTAKB" > $QUOTAFILE
  fi
}

fixeach() {
  read
  while [ $? -eq 0 ] ; do
    quotafile user.$REPLY
    if [ -f $QUOTAFILE ] ; then
      fixquota
      displayquota
    fi
    read
  done 
}

displayeach() {
  read
  while [ $? -eq 0 ] ; do
    quotafile user.$REPLY
    if [ -f $QUOTAFILE ] ; then
      displayquota
    fi
    read
  done 
}

FIXQUOTA=0
SHOWHEADER=1
setconfigdirectory /etc/imapd.conf init
# checkid
while [ x$1x != "xx" ] ; do
  if [ $1 = "-C" ] ; then
    shift
    setconfigdirectory $1
  elif [ $1 = "-f" ] ; then
    FIXQUOTA=1
  else
    quotafile $1
    if [ -f $QUOTAFILE ] ; then
      if [ $FIXQUOTA -eq 1 ] ; then
        fixquota
      fi
      if [ $SHOWHEADER -eq 1 ] ; then
        displayheader
        SHOWHEADER=0
      fi
      displayquota
    fi
  fi
  shift
done
if [ $SHOWHEADER -eq 1 ] ; then
  displayheader
  if [ $FIXQUOTA -eq 1 ] ; then
    ls $partition/user | fixeach
  else
    ls $partition/user | displayeach
  fi
fi

