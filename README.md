# Factions Plugin
Add factions and relations in your game using C++ or Blueprints

This is a plugin for Unreal Engine 4, and has support for versions **4.19** and **4.18**.

## Introduction

### What are "Factions"
When we say Faction we refer to a **group of entities or actors** that share something in common.

Almost every single type of game uses factions in different ways. For example, in Shooter games there will be enemies and friends, in RTS games every player will be a faction by itself, and in Open-World games you will have factions fighting each other while you run around.

This plugin fulfills the needs of this feature in UE4 with a very flexible tool that will make the implementation, editing and design of your own factions a 5 minutes thing.


### "What" can have a faction?
*Everything!*... well not everything, but pretty much. **All actors of any type can have a faction.**

## Installation
### Manually
This are the general steps for installing the plugin into your project:

**1.** Download the last release from [here](https://github.com/PipeRift/FactionsExtension/releases)<br>
*Make sure you download the same version than your project*

**2.** Extract the folder “FactionsExtension” into the **Plugins folder** of your existing project (e.g "MyProject/Plugins")

**2.** Done! You can now open the project

### From Marketplace
Not yet. Soon!

## Usage

Factions can be edited from *Project Settings*, inside *Game* category, *Factions* tab.
<p><img width="600" height="100%" src="https://i.imgur.com/5hKfoHF.png"></p>

### Editing Factions
Factions are defined as an array to be edited and every faction will have its own properties like color or default attitudes.

<img width="400" height="100%" src="https://i.imgur.com/YRl4Kmk.png">
<p>
  This information is available on runtime though<br>
  <img width="200" height="100%" src="https://i.imgur.com/4SOhCGb.png">
</p>

### Editing Relations
Relations define how will two factions interact to each other.

<img width="400" height="100%" src="https://i.imgur.com/ihngM3D.png">

By default there are three possible attitudes: **Hostile**, **Neutral** and **Friendly**

## Implementation
Patience! Will be written soon.
### Blueprints
### C++
