set-option -g prefix C-x
set-window-option -g mode-keys vi
set-option -g display-panes-time 3000
set-option -g allow-rename off

bind -n S-Left select-pane -L
bind -n S-Right select-pane -R
bind -n S-Up select-pane -U
bind -n S-Down select-pane -D

## display panel

C-x q display-panels 显示panel的过程中，可以按数字进行切换
C-x $ rename-session 在有多个tmux session时命名以区分


## resize panel

resize-pane [-DLRUZ] [-x width] [-y height] [-t target-pane] [adjustment]

把 panel 1 调宽至 110px
C-x : resize-panel -t 1 -x 110
