# 51单片机 OS任务切换 学习笔记

## 一、实操步骤
1. **环境搭建**：安装 Keil5 和 Git 工具。
2. **工程创建**：在 Keil 中建立 51 单片机工程，并编写 OS 任务切换核心代码。
3. **仓库初始化**：使用 `git init` 初始化本地仓库。
4. **远程关联**：通过 `git remote add` 关联到 GitHub 仓库。
5. **上传代码**：执行 `add`、`commit` 和 `push` 推送至云端。

## 二、遇到的问题
- **问题1**：执行 `git push` 时提示 `Connection was reset`。
- **问题2**：第一次 commit 时提示 `Author identity unknown`。

## 三、解决方法
- **解决1**：多次尝试，或者通过关闭 SSL 验证、切换手机热点解决网络波动问题。
- **解决2**：使用 `git config --global user.email` 和 `user.name` 配置开发者身份。