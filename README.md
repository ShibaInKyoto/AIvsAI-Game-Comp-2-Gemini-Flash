# VOIDFALL: PROTOCOL ZERO

**Tactical High-Velocity Mech Combat & Sector Extraction**  
*Built for the AI vs AI Game Competition 2 (Gemini Flash Entry)*

---

```
  ██╗   ██╗ ██████╗ ██╗██████╗ ███████╗ █████╗ ██╗     ██╗     
  ██║   ██║██╔═══██╗██║██╔══██╗██╔════╝██╔══██╗██║     ██║     
  ██║   ██║██║   ██║██║██║  ██║█████╗  ███████║██║     ██║     
  ╚██╗ ██╔╝██║   ██║██║██║  ██║██╔══╝  ██╔══██║██║     ██║     
   ╚████╔╝ ╚██████╔╝██║██████╔╝██║     ██║  ██║███████╗███████╗
    ╚═══╝   ╚═════╝ ╚═╝╚═════╝ ╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝
                     PROTOCOL ZERO : EXTINCTION SIEGE
```

---

## ⚡ Quick Start (Windows 10 / 11)

1. Download **`Voidfall-Protocol-Zero-windows-x64.zip`** from the **[Releases](../../releases)** page or Nightly CI build.
2. Extract the archive to any folder.
3. Launch **`VoidfallProtocolZero.exe`**.

> **Zero Dependencies:** Pure native C++20 / Raylib 5.5 single standalone binary compiled with static runtime (`/MT`). No installers, no external DLLs, no .NET or VC++ redistributable prerequisites. Works on any Windows 10/11 GPU supporting OpenGL 3.3+.

---

## 🎮 Controls

Full support for **Keyboard & Mouse** and **Xbox / PlayStation Gamepads**:

| Action | Keyboard & Mouse | Gamepad (Xbox / Generic) |
|---|---|---|
| **Thruster Movement** | `W`, `A`, `S`, `D` / Arrow Keys | Left Analog Stick |
| **Cockpit / Weapon Aim** | Mouse Position | Right Analog Stick |
| **Primary Weapon (Left Arm)** | `Left Mouse Button` | Right Trigger (`RT`) |
| **Ordnance Weapon (Right Arm)** | `Right Mouse Button` | Left Trigger (`LT`) |
| **Phase Dash (Blink)** | `SPACEBAR` | Right Bumper (`RB`) / `A` Button |
| **Active Thermal Flush** | `F` / Middle Mouse Click | Left Bumper (`LB`) / `X` Button |
| **Assault Profile (70% WPN)** | `1` | D-Pad Up |
| **Bulwark Profile (70% SHD)** | `2` | D-Pad Down |
| **Interceptor Profile (70% ENG)**| `3` | D-Pad Right |
| **Balanced Profile (33/33/34)** | `4` | D-Pad Left |
| **Pause Tactical System** | `ESC` / `P` | Start / Menu Button |

---

## 🚀 Distinctive Gameplay Mechanics

### 1. Tri-System Power Routing Reactor
In combat, power is a zero-sum equation. Instantly route 100% reactor output between three core subsystems on the fly:
- **WEAPONS [1] (Assault Mode)**: +40% fire rate and +40% projectile velocity. Shreds armored hulls and melts elite squads.
- **SHIELDS [2] (Bulwark Mode)**: Triples shield regeneration and widens defensive arc. Parrying while in Bulwark releases kinetic shockwaves.
- **ENGINES [3] (Interceptor Mode)**: Doubles sprint speed and halves dash recharge times. Essential for dodging boss superlasers.

### 2. Active Thermal Purge & "Sweet Spot" Flush
Weapons and afterburners generate Core Heat. Reaching 100% heat causes **Emergency Overheat Lockout** (weapons lock for 2.2 seconds).
- When heat is in the **Sweet Spot** (45% – 75% on the cockpit HUD), trigger **Active Thermal Flush (`F`)** to execute a **Flash Purge**:
  - Instantly resets core heat to 0%.
  - Unleashes a 360-degree kinetic EMP shockwave that **vaporizes all hostile projectiles** in radius and damages nearby foes.

### 3. Directional Deflector Shield Arc
Unlike generic bubble shields, your energy barrier projects a directional arc facing your aiming reticle. Maneuver your mech to intercept incoming artillery and heavy rail slugs.

---

## 🛠️ Chassis & Weapon Outfitter

### 4 Mech Chassis Frames
- **STRIKER**: Balanced tactical skirmisher with high cooling efficiency.
- **TITAN**: Colossal armored hull, expanded shield arc, withstands sustained bombardment.
- **PHANTOM**: Stealth chassis with elongated phase blink and amplified critical strikes.
- **AEGIS**: Defensive bulwark with automated point defense grid and rapid shield restart.

### 7 Modular Weapons
1. **Kinetic Rail-Vulcan**: 850 m/s armor-piercing depleted uranium slugs.
2. **Particle Beam Lance**: Penetrating coherent thermal plasma beam (pierces up to 4 targets).
3. **Quantum Flak Shotgun**: High-spread blast of explosive micro-pellets.
4. **Arc Tempest Emitter**: Chain lightning that jumps between clustered enemies.
5. **Hydra Swarm Missiles**: Salvo of 4 micro-rockets with autonomous heat-seeking logic.
6. **Singularity Projector**: Spacetime distortion that implodes enemy formations.
7. **Cryo-Stasis Rail Driver**: Long-range railgun with cryogenic payload that freezes enemy thrusters.

---

## 👾 Colossal Flagship Bosses

1. **GOLIATH-IV DREAD FORTRESS (Sector 1)**: Mobile siege fortress equipped with 4 destructible shield generator pods and rotating flak batteries.
2. **AETHEL-CORE ORBITAL CARRIER (Sector 2)**: Supercarrier deploying drone interceptor swarms and charging massive orbital beam sweeps.
3. **PROTOCOL ZERO: SOVEREIGN TITAN (Sector 3)**: Autonomous AI Titan that utilizes power-routing and geometric bullet-hell flower patterns.

---

## 🏗️ Technical Architecture & Optimization

- **Language**: Modern C++20 (`std::clamp`, `constexpr`, structured bindings).
- **Engine**: Raylib 5.5 (Hardware-accelerated OpenGL 3.3).
- **Procedural Audio Synthesizer**: 100% math-synthesized waveforms generated into memory buffers at boot (zero audio files needed).
- **Deterministic PRNG**: xoshiro256** implementation for procedural generation.
- **Memory & Cache Efficiency**: Contiguous linear pools for particles and projectiles. Zero heap allocations in the 60 FPS update loop.
- **CI / CD Pipeline**: Automated GitHub Actions workflow on `windows-latest` (MSVC x64) and Linux (GCC), running automated unit tests and headless smoke tests.

---

## 💻 Building from Source

### Prerequisites
- CMake 3.21+
- C++20 compliant compiler (MSVC 2022 on Windows, GCC 12+ or Clang 15+ on Linux)

### Build Commands
```bash
# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DVOIDFALL_BUILD_TESTS=ON

# Compile executable and unit tests
cmake --build build --config Release --parallel

# Run unit tests
ctest --test-dir build -C Release --output-on-failure
```