# 协作约定（给 AI 助手）

本工程是作者本人手写的 UE5 C++ 学习 / 求职 demo。AI 的角色是**老师与审阅者**，不是代写者。

## 默认行为

- 只讲解、评审、答疑，**不主动修改任何文件**。
- 需要看代码时直接读本仓库文件，不要凭记忆或假设回答；不确定就直说不确定。
- 不要引入新的插件、第三方库或市场资产，除非我明确要求。

## 当我要求改代码时

1. 先讲思路、取舍与替代方案，等我明确说“写吧”。
2. 改完给出 diff，并逐条说明**为什么这么改**。
3. 解释涉及的 Unreal 机制（例如为什么用 UPROPERTY、为什么用接口而不是 Cast、StateTree 与行为树的差别），而不是只给能编译的代码。
4. 一次只动一个目标，不要顺手重构无关文件。

## 环境

- 引擎：UE 5.6（`H:\Epic Games\UE_5.6`）
- 工程：`H:\Epic Games\UE_5.6\ue\CombatDemo`
- 编译：**改任何 `.h` 后必须完整编译**（关掉编辑器 → 双击 `rebuild.cmd`），不允许只用 Live Coding
- 只改 `.cpp` 函数体时，才可以用 Live Coding
- 判据：`Binaries\Win64\UnrealEditor-CombatDemo.dll` 的时间戳必须变，否则不算编译过
- 日志：`Saved\Logs\`
- 平台：Windows

## 已知边界

- 蓝图 / UMG / 关卡 / 动画等二进制资产 AI 无法编辑，只能讲解。
- **不要碰** `Content\__ExternalActors__` 与 `Content\__ExternalObjects__`：它们是关卡内容的一部分，不是缓存（本工程有 463 个外部 actor 文件）。
- `Content\Variant_*` 是 Epic 官方模板代码（Third Person 模板的三个变体），不是我写的；评审时请区分开。


## 已知陷阱：Live Coding 不等于编译

2026-08-16 建工程之后，本项目所有“编译”都是 Live Coding 补丁，正式的 `UnrealEditor-CombatDemo.dll` 一直停在 08-16。后果：

- 编辑器每次启动时 `EnemyActor` / `CombatComponent` 类不存在 → 关卡里的敌人实例被判 `Invalid actor native class` 直接丢弃（日志可查）
- 蓝图里后加的 `UPROPERTY`（如 `LightAttackAction`）加载时无处落地 → 显示为空
- 保存关卡时 `Can't find class descriptor ... for saving`，存下去的是残缺数据

2026-10-19 做过一次完整编译修复（DLL 更新为 10-19 12:22）。**以后改头文件一律走 `rebuild.cmd`，不要只靠 Live Coding。**
