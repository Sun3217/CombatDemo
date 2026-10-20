# CombatDemo — UE5 C++ 第三人称动作战斗 Demo

用 **Unreal Engine 5.6 + C++** 实现的第三人称动作战斗原型：玩家可以移动、攻击，敌人会追击并攻击玩家；双方都有血条、受击反馈和死亡表现。

> 🎬 演示视频（50 秒）：[https://www.bilibili.com/video/BV1VLnc6yEDH/](https://www.bilibili.com/video/BV1VLnc6yEDH/)
> 🎯 定位：C++ 初级 / UE 游戏客户端方向的作品集项目

---

## 演示内容

演示视频共 50 秒，包含两段：

1. **击杀敌人** — 进入敌人攻击范围后敌人立即起手；玩家正面攻击，敌人血条清零后切换为布娃娃物理倒地。
2. **死亡与重生** — 被敌人连续命中，玩家血条下降并触发受击震屏；血量归零后玩家布娃娃倒地、镜头拉远俯视、画面渐黑，数秒后关卡自动重开，形成完整战斗闭环。

<!-- GIF 做好后放入 Docs/demo.gif，然后取消下面一行的注释 -->
<!-- ![Combat Demo](Docs/demo.gif) -->

## 操作

| 输入 | 行为 |
|---|---|
| `W A S D` | 移动 |
| 鼠标 | 视角 |
| `空格` | 跳跃 |
| `鼠标左键` | 攻击 |

---

## 已实现功能

- **玩家角色**：第三人称移动 / 视角 / 跳跃（Enhanced Input）
- **战斗组件**：`UCombatComponent` 独立管理玩家 HP、攻击力、攻击范围，与角色类解耦
- **攻击判定**：水平距离（`FVector::Dist2D`）+ 前方 ±60° 扇形（点积），只命中前方最近的敌人
- **敌人 AI**：枚举状态机（Idle / Chase / Attack / Dead），进入攻击范围立刻起手，按固定间隔循环攻击
- **伤害结算**：敌人攻击命中后回调玩家组件的 `ApplyDamage`
- **UI**：敌人血条（世界空间 `UWidgetComponent`）、玩家血条（屏幕空间 UMG）
- **受击反馈**：摄像机震屏（`BP_CameraShake_Hit_Player`）
- **死亡表现**：双方死亡都切换为布娃娃物理；玩家死亡后禁用输入、画面渐黑、数秒后自动重开
- **动画细节**：攻击蒙太奇播放期间关闭脚部 IK，避免 IK 把抬起的腿锁回地面

---

## 技术要点

1. **组件化而非继承**
   战斗能力放在 `UCombatComponent`，角色只负责表现与输入。后续加背包 / Buff / 装备可以各自独立扩展。

2. **攻击判定为什么用点积**
   `Dot(Forward, ToEnemy) > 0.5` 即 ±60° 扇形。相比纯距离判定多了方向约束，而且不需要额外碰撞体。

3. **近战判定为什么要用水平距离**
   `FVector::Dist2D` 忽略高度差，避免上下台阶、跳跃时判定距离漂移。

4. **状态机**
   目前是枚举 + `switch`，4 个状态足够清晰；状态数增长后会迁移到 StateTree（项目已启用该插件）。

5. **布娃娃死亡踩过的两个坑**
   - `ACharacter` 的 Mesh 默认碰撞预设是 `CharacterMesh`（`QueryOnly`），直接开物理会穿过地面 → 死亡时必须改成 `Ragdoll`
   - 动画蓝图（含脚部 IK）会和物理抢骨骼控制权 → 死亡时停掉 AnimBP，让物理完全接管

6. **脚部 IK 与攻击动画的冲突**
   AnimBP 里 `Slot` 节点在 `Control Rig` 之前，所以 IK 会后运行并把蒙太奇抬起的腿锁回地面。解决方式是用「是否有蒙太奇在播放」驱动 `Control Rig` 的 `Alpha`，攻击时让 IK 权重归零。

---

## 目录结构

```
Source/CombatDemo/
  CombatComponent.h/.cpp      # 玩家战斗组件（HP / 攻击 / 受击 / 血条 UI）
  CombatDemoCharacter.*       # 玩家角色（输入绑定、攻击蒙太奇）
  EnemyActor.*                # 敌人（状态机、血条、伤害结算、布娃娃死亡）
  Variant_*                   # Epic 官方模板变体（非本人编写，见下方说明）
```

---

## 运行方式

1. 需要 **Unreal Engine 5.6**
2. 右键 `CombatDemo.uproject` → *Generate Visual Studio project files*
3. 用 VS2022 / Rider 编译 `CombatDemoEditor`（`Development` / `Win64`）
4. 打开工程，进入 `Content/ThirdPerson/Lvl_ThirdPerson`，按 **Play**

> 工程用 **Git LFS** 管理 `.uasset` / `.umap` / `.fbx`，克隆前请先安装 Git LFS 并执行 `git lfs install`。

---

## 关于模板

第三人称移动、角色模型与部分动画资产来自 **Epic 官方 Third Person 模板及其 Combat 变体**。
`Source/CombatDemo/Variant_Combat`、`Variant_Platforming`、`Variant_SideScrolling` 三个目录为模板代码，文件头保留 Epic 版权声明。

本人在此基础上编写的是：

- 战斗组件 `UCombatComponent`（血量、攻击、受击、血条刷新）
- 攻击判定（水平距离 + 前方扇形 + 最近目标筛选）
- 敌人状态机、攻击节奏与伤害结算
- 双方血条 UI、受击震屏
- 玩家与敌人的死亡表现、玩家死亡后的重生流程
- 脚部 IK 与攻击动画的互斥处理

---

## 已知限制 / 后续方向

- **命中判定**目前是「距离 + 扇形」，没有按攻击帧做碰撞检测。计划改为 `AnimNotify` + 球形 Overlap，做到"看攻击帧、看部位"。
- **伤害结算用的是定时器**（挥拳后 0.45 秒判定），不是攻击帧事件。所以敌人动画被卡住时仍然会造成伤害 —— 这也是上一条要改成 `AnimNotify` 的直接原因。
- **敌人没有寻路**，被墙挡住时可能卡在角落。计划接入 NavMesh + `MoveTo`。
- **敌人 AI** 尚未使用行为树 / StateTree，目前是手写枚举状态机。
- 只有一个敌人模板，没有做多敌人编队 / 仇恨管理。
- **跳到敌人头上时**，两个胶囊体互相挤压会把敌人顶离地面，`CharacterMovement` 因此判定为 `Falling`，敌人动画会卡在"下落"姿势。属于非预期操作的边缘情况，未做处理。
