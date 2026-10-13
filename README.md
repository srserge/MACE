# MACE — Modular Advanced Combat Engine

A melee combat system for **Unreal Engine 5.8**, built as a set of portable plugins on top of the
Gameplay Ability System.

> **Work in progress.** The melee combat chain is playable end to end. Ranged combat, AI, blocking
> and the debug tools are still ahead.

---

## What it does today

- **Combos driven by data.** Each blow is a DataAsset (montage, damage, trace sockets, impact
  force); a combo is an ordered list of blows; a Chooser table picks which combo to throw from the
  context of the fight. Chaining happens inside a window marked on the animation, with input
  buffering so early presses are not lost.
- **Hit detection by body zone.** Hurtboxes are authored as components on bones, then read into
  plain data and destroyed at `BeginPlay`, so a character nobody is hitting costs nothing. Swept
  sphere traces resolve which zone each blow lands on.
- **Damage by zone.** A GAS execution calculation multiplies the attack's base damage by the
  target's rule for the struck zone: a headshot hurts more than a blow to the arm.
- **Hit reactions.** Direction and zone pick the reaction montage through a Chooser table, and a
  partial physics blend on the upper body pushes the struck bone before blending back to the
  animation.
- **Death.** A ragdoll with optional physical animation that fades out, carrying the momentum of the
  killing blow. The corpse stays in the world, can be pushed around and stops costing CPU once it
  settles.

## Architecture

```
MaceSample  ──▶  MaceAnimation  ──▶  MaceCore  ◀──  MaceDebug
```

| Plugin | Role | Status |
|---|---|---|
| `MaceCore` | The contract: base DataAssets, attributes, components, gameplay tags, interfaces | In progress |
| `MaceAnimation` | Bridge to PoseSearch, Chooser and Motion Warping. Deliberately expendable | Planned |
| `MaceSample` | The gym: characters, abilities, DataAssets and level. No code | Lives in `Content/Sandbox` for now |
| `MaceDebug` | In-game debug menu | Planned |

Two rules hold the structure together:

1. **No plugin references `/Game/`.** A plugin that points at project content stops being
   portable, and nothing tells you until you try it elsewhere.
2. **Dependencies only point towards the core.** `MaceCore` depends on nothing experimental:
   PoseSearch, Chooser, Mover and Motion Warping are kept out of it on purpose.

### Design decisions worth knowing

- **Abilities are authored in Blueprint** on top of C++ base classes. C++ provides the framework;
  designers own the behaviour.
- **The Ability System Component lives on the pawn**, not on the PlayerState, because enemies have
  no PlayerState. It is always reached through `IAbilitySystemInterface`, never by casting.
- **The combat style is derived, never stored.** It comes from the equipped weapon, falling back to
  the character's base style. Stances such as blocking or aiming are gameplay tags, not styles, so
  they never grant or revoke abilities.
- **DataAssets are never written at runtime.** In Play In Editor they are not duplicated, so a
  write would leak into the editor asset between sessions.
- **Impact classes and hit zones are gameplay tags, not enums**, so a game built on MACE can add its
  own without recompiling the core. Zones are hierarchical: `Mace.HitZone.Limb.Arm.Left` is also a
  `Mace.HitZone.Limb`.
- **Network-ready, not network-tested.** Damage is server-authoritative and every attribute change
  goes through a GameplayEffect.

## Repository layout

```
MaceEngine/
├── Plugins/MaceCore/     The core plugin: C++ only
├── Content/Sandbox/      Test content: abilities, characters, DataAssets, Chooser tables
├── Config/               Collision channels, gameplay tags, CVars
├── Source/MaceEngine/    The host project module, minimal on purpose
└── MaceEngine.uproject
```

The project is the host, not the product: the plugins are what MACE ships.

---

## Getting started

### Requirements

| | |
|---|---|
| Engine | Unreal Engine **5.8.1** |
| IDE | Visual Studio 2022 or later with *Game development with C++*, or Rider |
| Content | **Game Animation Sample Project (GASP) 5.8**, from the Epic Games Launcher |
| Version control | Git with **Git LFS** (`git lfs install`) |

### 1. Bring in the GASP content

The character, locomotion and level come from Epic's Game Animation Sample Project, which is not
versioned here. Install GASP 5.8, open it, select these folders in the Content Browser and
**Migrate…** them into `<clone>/MaceEngine/Content`, with the MaceEngine editor closed:

```
Content/Blueprints
Content/Characters/UEFN_Mannequin
Content/Characters/UE5_Mannequins
Content/Input
Content/Audio
Content/Widgets
Content/Levels/LevelPrototyping
```

The target must end in `\Content`; picking the project root produces `Content\Content\…`.
`LevelPrototyping` is required: it defines `LevelBlock_Traversable`, without which
`AC_TraversalLogic` does not compile.

### 2. Build

Right-click `MaceEngine.uproject` → *Generate Visual Studio project files*, then build
`MaceEngineEditor | Development Editor | Win64`.

### 3. Animations not included

The attack, locomotion and hit reaction animations come from third-party packs whose licences do
not allow redistribution, so they are not in this repository. The project opens and the systems are
all there, but montages that reference those packs will be missing until you provide your own.

---

## Licence

© srSerge. All rights reserved. The source is published for viewing.

The Game Animation Sample Project is owned by Epic Games and used under the Unreal Engine content
licence. It lives in `Content/` and never inside `Plugins/`, so the plugins can be distributed
without it.
