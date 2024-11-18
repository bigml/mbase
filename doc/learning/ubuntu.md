### 添加应用到 dock 栏

$ vim .local/share/applications/emacs29.desktop

[Desktop Entry]
Encoding=UTF-8
Name=Emacs29
Comment=build version
Exec=/usr/local/emacs/emacs-29.4/src/emacs
Icon=/usr/local/emacs/emacs-29.4/etc/images/icons/hicolor/48x48/apps/emacs.png
Terminal=false
Type=Application
Categories=Development
StartupWMClass=emacs

$ gsettings get org.gnome.shell favorite-apps
$ gsettings set org.gnome.shell favorite-apps "['emacs29.desktop', '....原有的.desktop']"

其中 StartupWMClass 的值通过终端 $ xprop WM_CLASS 然后十字鼠标点击程序窗口获取。
