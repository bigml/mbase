## 关于 commit, tag, changelog

* git commit -m "" 是每次提交必做的工作，简要说明本次提交

* 对于较多内容的提交（多于一句简要的说明，小于一次重要的版本更新）需要较多的说明文字时，
  全部内容建议写在 changelog 文件里

* 对于重要的版本更新，建议使用 git tag vx.y.z 对此次提交打上标签，并将全部内容写在 changelog 文件里

故，changelog 用来记录较复杂的 commit message，git tag 用来标记重要的版本更新。
一般来说，不是每条 commit 都对应了 changelog 中的条目，但每条 tag 都会对应 changelog 中的条目。
