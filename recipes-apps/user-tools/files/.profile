

# enable color support of ls and also add handy aliases
if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval "$(dircolors -b ~/.dircolors)" || eval "$(dircolors -b)"
    alias ls='ls --color=auto'
    alias grep='grep --color=auto'
    alias fgrep='fgrep --color=auto'
    alias egrep='egrep --color=auto'
fi

alias ll='ls -lat'
alias cd..='cd ..'

echo "#################################################"
echo "#    #######                               "
echo "#         #  #    # #    # # #    #  ####  "
echo "#        #   #    # #   #  # ##  ## #    # "
echo "#       #    #    # ####   # # ## # #    # "
echo "#      #     #    # #  #   # #    # #    # "
echo "#     #      #    # #   #  # #    # #    # "
echo "#    #######  ####  #    # # #    #  ####  "
echo "#"
echo "# Welcome to the zukimo board."
echo "#################################################"

