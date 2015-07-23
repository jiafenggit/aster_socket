#!/bin/sh

commit_text=$1

if [ -n "$commit_text" ]; then
	git add .
	git commit -m "$commit_text"
	git pull origin master
	git push origin master
else
	echo "Input commit text. Example: ./git.sh commit_text"
fi

exit 0
