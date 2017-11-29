# note
note files
1，生成key
 先输入ssh-keygen –t rsa –C “邮箱地址”,注意ssh-keygen之间是没有空格的,其他的之间是有空格的
2,id_rsa.pub内容添加到github
3,ssh –T git@github.com 连接测试
4，配置邮箱
git config –global user.name “用户名”
git config –global user.email “邮箱”
5，初始化工作目录
git init
6，
增加对我们github上创建的buxingxing仓库的管理。
git remote add origin git@github.com:bxxfighting/buxingxing.git
其中bxxfighting是我在网站上注册时使用的用户名，buxingxing.git是我为这个项目建立的仓库名
7，git pull git@github.com:bxxfighting/buxingxing.git 向下同步数据
8，git push git@github.com:bxxfighting/buxingxing.git 向上同步数据

