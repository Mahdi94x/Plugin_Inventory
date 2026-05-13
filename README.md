# Plugin_Inventory

Developed with Unreal Engine 5.7  

Based on the Udemy Course Unreal Engine 5 C++ Inventory Systems by Stephen Ulibarri  

Plugin_Inventory is a modular multiplayer-ready inventory framework built in Unreal Engine using C++ and Blueprints.
The project focuses on scalable inventory architecture, reusable item systems, and flexible gameplay-driven item interactions using modern Unreal Engine C++ practices.

The inventory system is implemented as a standalone plugin architecture, allowing the system to be reused across multiple projects while maintaining clean separation between gameplay systems and inventory functionality.  

# Core Features  
* Spatial Inventory System
   * Grid-based spatial inventory system
   * Multiple inventory categories and switchable grids
   * Support for Equippable, Consumable, and Craftable item types
   * Variable-sized items occupying multiple grid cells
   * Item movement and placement validation inside grids
   * Dynamic inventory interactions with drag-and-drop behavior

# Modular Item Architecture
* Item Fragment System
   * Modular Fragment-based item design
   * Items gain functionality through attached Fragments
   * Grid Fragments for spatial placement behavior
   * Stackable Fragments for stack logic
   * Equippable Fragments for equipment functionality
   * Consumable Fragments for gameplay effects and usage
   * Flexible architecture allowing easy extension with new item behaviors

# Inventory Interaction & Item Logic  
* Stackable Items  
   * Stack counts with configurable maximum stack sizes  
   * Splitting item stacks  
   * Combining stacks  
   * Dropping items into the world  
   * Inventory validation for stack operations

* Consumable Items  
   * Functional consumable items with custom behaviors
   * Potion consumption system
   * Gameplay-driven consume logic
   * Flexible structure for adding additional consumable effects
  
* Equipment System
   * Equipment slot system for wearable and usable items
   * Equipping cloaks, masks, and weapons
   * Item-specific equip logic
   * Functional equipment affecting gameplay systems rather than only UI representation
  
# UI & Item Presentation
* Inventory User Interface
   * Dynamic inventory menus built with Unreal Engine UI systems
   * Item tooltip popups displaying contextual item data
   * Tooltips adapt dynamically based on Item Fragments
   * Flexible UI architecture for future expansion
   * Visual feedback for inventory interactions
 
# Multiplayer & Networking
* Multiplayer-Ready Architecture
   * Inventory functionality built for multiplayer environments
   * Compatible with Listen Servers and Dedicated Servers
   * Replicated inventory and item interactions
   * Multiplayer-safe item movement and usage
   * Single-player and standalone support
 
# Plugin-Based Architecture
* Reusable Inventory Plugin
   * Inventory system implemented as a separate Unreal Engine plugin
   * Designed for portability between projects
   * Modular code organization
   * Clean separation between inventory logic and game-specific systems
   * Scalable architecture following Unreal Engine C++ best practices
 
# Gameplay Integration
* Flexible Gameplay Systems
   * Functional item interactions tied directly to gameplay systems
   * Extensible framework for custom item usage behavior
   * Designed for integration with Gameplay Ability System (GAS)
   * Event-driven gameplay interactions
   * Data-driven item functionality
 
# Visual Effects & Assets
* Visual Feedback
   * Niagara effects for item pickups
   * Custom item meshes and inventory assets
   * UI textures, borders, widgets, and icons
   * Inventory interaction polish and feedback systems
 
# Tools & Technologies
   * Unreal Engine 5
   * C++ and Blueprints
   * Unreal Engine Plugin Architecture
   * Multiplayer Networking
   * Niagara VFX
   * Spatial Inventory Systems
   * Modular Item Fragment Architecture
   * UI Systems and Tooltips
 
# Learning Focus
   * Designing scalable inventory architectures in Unreal Engine
   * Multiplayer inventory replication and synchronization
   * Modular gameplay programming patterns
   * Plugin-based Unreal Engine development
   * Hybrid C++ / Blueprint workflows
   * Clean code organization for gameplay systems
   * Data-driven and extensible item systems
