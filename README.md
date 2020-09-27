# puppetwork

## 使用说明

- 仅能使用信息拉取的功能。基本支持全部信息类型的拉取，但是一些种类的信息将转换成普通文本信息存储。
- 推荐使用版本为8.7.0的node.js（即开发环境），否则依赖包node-ffi将出现不可控的错误。
- 媒体文件拉取到后将先放入cache文件夹下，产生一份文件。目前没有制定清理该文件夹的功能，可以手动清理该文件夹。

## 配置说明

- 将rsa的private key放入private_key.txt中，将corpid（调用企业的企业id，例如：wwd08c8exxxx5ab44d，可以在企业微信管理端--我的企业--企业信息查看）与secret（聊天内容存档的Secret，可以在企业微信管理端--管理工具--聊天内容存档查看）按照json格式放入parameters.json中
- 正常运行box即可

## 不足与进一步开发说明

- message可以扩充type类，将更多消息分类。（目前多使用text类封装）
- 媒体文件无法进行异步获取。调用messageFIle的时候需要等待媒体文件完全下载才可以进行下一步。
- 媒体文件未进行md5sum校验。（c代码已经写入了，但是目前转换有bug，因此没有接入）
- messageImage、messageMiniProgram、messageUrl等函数仍未实现。
- 设置开始拉取的位置。（现在每次都从seq=0开始拉取）
- 一次拉取多条消息。（现在每次从网上拉取一点）
- contact和room类仅封装了id等基本信息。
- 可以在linux系统下测试。

## 补充说明
- GitHub中的项目尚未调试，不保证可以直接运行。原本的开发环境下，example中的测试用代码与src中的puppet-work.ts代码及.dll文件与package.json等文件在同一个目录下。为了清晰起见，将代码大致作了一些区分，放在不同的文件夹下。
