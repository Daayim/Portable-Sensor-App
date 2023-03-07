#***************************************************************************
# add_sha1.sh:  Bash script to write the current Git SHA1 id to
#	sha1.txt
#
# Written by:
# 	Sandra Mercer
# 	Engineer / Software Developer
# 	eSonar Inc.
#
# Date:
# 	November 2014
#***************************************************************************

git log -n 1 --abbrev=8 --pretty="%h" > sha1.txt

