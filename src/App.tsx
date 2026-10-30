import React, { useState } from 'react';
import { Shield, Zap, Flame, Crosshair, Terminal, Cpu, Award, Download, Gamepad2, Layers, Map, Compass, Target, Trophy, Radio, Activity, Wrench } from 'lucide-react';

export default function App() {
  const [activeTab, setActiveTab] = useState<'overview' | 'campaign' | 'telemetry' | 'endless' | 'hangar' | 'weapons' | 'reactor' | 'ci'>('overview');
  const [profile, setProfile] = useState<'BALANCED' | 'ASSAULT' | 'BULWARK' | 'INTERCEPTOR'>('BALANCED');

  const allocations = {
    BALANCED: { wpn: 33, shd: 33, eng: 34, desc: 'Standard operating baseline with equal resource distribution across all systems.' },
    ASSAULT: { wpn: 70, shd: 15, eng: 15, desc: '+40% projectile fire rate & kinetic damage. Maximizes DPS against colossal flagships.' },
    BULWARK: { wpn: 15, shd: 70, eng: 15, desc: 'Tripled shield recharge rate & expanded arc. Perfect parries emit kinetic shockwaves.' },
    INTERCEPTOR: { wpn: 15, shd: 15, eng: 70, desc: 'Double sprint velocity & halved dash recovery. Required for dodging orbital superlasers.' },
  };

  const weapons = [
    { name: 'Kinetic Rail-Vulcan', slot: 'Primary', desc: 'Rapid-fire depleted uranium kinetic slugs with hyper velocity.', color: '#fbbf24', speed: '850 m/s' },
    { name: 'Particle Beam Lance', slot: 'Primary', desc: 'Coherent thermal plasma bolt that pierces multiple hull plates.', color: '#38bdf8', speed: '1600 m/s' },
    { name: 'Quantum Flak Cannon', slot: 'Primary', desc: 'Fires a devastating arc of explosive micro-shrapnel pellets.', color: '#fb923c', speed: '680 m/s' },
    { name: 'Arc Tempest Emitter', slot: 'Primary', desc: 'High-voltage lightning discharges that shatter energy barriers.', color: '#c084fc', speed: '950 m/s' },
    { name: 'Neutron Disruptor', slot: 'Primary', desc: 'Continuous high-frequency beam that pierces shields and destabilizes cores.', color: '#4ade80', speed: '1450 m/s' },
    { name: 'Hydra Swarm Missiles', slot: 'Ordnance', desc: 'Salvo of micro-rockets with autonomous heat-seeking guidance.', color: '#f43f5e', speed: '480 m/s' },
    { name: 'Singularity Projector', slot: 'Ordnance', desc: 'Compresses spacetime into an imploding gravity well.', color: '#a855f7', speed: '380 m/s' },
    { name: 'Cryo-Stasis Rail Driver', slot: 'Ordnance', desc: 'Extreme-range magnetic accelerator with cryogenic payload.', color: '#22d3ee', speed: '1900 m/s' },
    { name: 'Nuclear Cluster Torpedo', slot: 'Ordnance', desc: 'Colossal fusion warhead that shatters into 8 radial submunitions.', color: '#ea580c', speed: '320 m/s' },
  ];

  const upgrades = [
    { title: 'Hull Composite Reinforcement', desc: '+15 Max Hull Integrity per level (Max 5 levels)', cost: '150 - 750 Nanites', icon: '🛡️' },
    { title: 'Cooling Turbines & Heat Sinks', desc: '+10% Faster Core Heat Dissipation per level (Max 5 levels)', cost: '150 - 750 Nanites', icon: '❄️' },
    { title: 'Shield Overcharge Capacitors', desc: '+10 Max Shield Capacity per level (Max 5 levels)', cost: '150 - 750 Nanites', icon: '⚡' },
    { title: 'Sub-Molecular Nanite Harvesters', desc: '+15% Extra Nanite drops per salvage salvage (Max 5 levels)', cost: '150 - 750 Nanites', icon: '💎' },
    { title: 'Phase Dash Capacitor Coils', desc: '+8% Faster Dash Thruster Cooldown per level (Max 5 levels)', cost: '150 - 750 Nanites', icon: '🚀' },
  ];

  return (
    <div style={{ minHeight: '100vh', background: '#080d1a', color: '#f1f5f9', fontFamily: 'system-ui, -apple-system, sans-serif' }}>
      {/* Header Bar */}
      <header style={{ borderBottom: '1px solid #1e293b', background: 'rgba(15, 23, 42, 0.85)', backdropFilter: 'blur(10px)', padding: '1rem 2rem', position: 'sticky', top: 0, zIndex: 50 }}>
        <div style={{ maxWidth: '1200px', margin: '0 auto', display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: '0.75rem' }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: '0.75rem' }}>
            <div style={{ width: '40px', height: '40px', borderRadius: '8px', background: 'linear-gradient(135deg, #6366f1, #06b6d4)', display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
              <Zap size={22} color="#ffffff" />
            </div>
            <div>
              <div style={{ fontSize: '1.25rem', fontWeight: 800, letterSpacing: '0.05em', color: '#f8fafc' }}>VOIDFALL: PROTOCOL ZERO</div>
              <div style={{ fontSize: '0.75rem', color: '#38bdf8', letterSpacing: '0.1em', textTransform: 'uppercase' }}>AI vs AI Game Competition 2 • Gemini Flash Entry</div>
            </div>
          </div>

          <div style={{ display: 'flex', gap: '0.4rem', flexWrap: 'wrap' }}>
            {(['overview', 'campaign', 'telemetry', 'endless', 'hangar', 'weapons', 'reactor', 'ci'] as const).map((tab) => (
              <button
                key={tab}
                onClick={() => setActiveTab(tab)}
                style={{
                  padding: '0.4rem 0.85rem',
                  borderRadius: '6px',
                  border: '1px solid',
                  borderColor: activeTab === tab ? '#6366f1' : '#334155',
                  background: activeTab === tab ? '#4f46e5' : 'transparent',
                  color: activeTab === tab ? '#ffffff' : '#94a3b8',
                  fontSize: '0.85rem',
                  fontWeight: 600,
                  cursor: 'pointer',
                  textTransform: 'capitalize'
                }}
              >
                {tab}
              </button>
            ))}
          </div>
        </div>
      </header>

      {/* Main Content Area */}
      <main style={{ maxWidth: '1200px', margin: '2rem auto', padding: '0 1.5rem' }}>
        {activeTab === 'overview' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '1.5rem' }}>
            <div style={{ background: 'linear-gradient(180deg, rgba(30, 41, 59, 0.7) 0%, rgba(15, 23, 42, 0.9) 100%)', border: '1px solid #334155', borderRadius: '12px', padding: '2rem', position: 'relative', overflow: 'hidden' }}>
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', flexWrap: 'wrap', gap: '1rem' }}>
                <div style={{ maxWidth: '720px' }}>
                  <div style={{ display: 'inline-flex', alignItems: 'center', gap: '0.5rem', background: 'rgba(56, 189, 248, 0.15)', color: '#38bdf8', padding: '0.25rem 0.75rem', borderRadius: '9999px', fontSize: '0.8rem', fontWeight: 600, marginBottom: '0.75rem' }}>
                    <Cpu size={14} /> Standalone Native Windows 10/11 Executable (C++20 & Raylib 5.5)
                  </div>
                  <h1 style={{ fontSize: '2.4rem', fontWeight: 900, margin: '0 0 0.75rem', lineHeight: 1.1, color: '#ffffff' }}>
                    Tactical High-Velocity Mech Combat & Sector Extraction
                  </h1>
                  <p style={{ color: '#94a3b8', fontSize: '1.05rem', lineHeight: 1.6, margin: '0 0 1.5rem' }}>
                    Trapped in deep space sector quarantine after the activation of Protocol Zero, you pilot an armored combat frame capable of real-time reactor power redirection, active thermal purges, and directional shield parrying.
                  </p>
                </div>

                <div style={{ background: '#090d16', border: '1px solid #1e293b', borderRadius: '8px', padding: '1.25rem', minWidth: '240px' }}>
                  <div style={{ fontSize: '0.8rem', color: '#64748b', textTransform: 'uppercase', fontWeight: 700, marginBottom: '0.75rem' }}>Native Desktop Metrics</div>
                  <div style={{ display: 'flex', flexDirection: 'column', gap: '0.6rem', fontSize: '0.9rem' }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: '#94a3b8' }}>Target:</span>
                      <strong style={{ color: '#38bdf8' }}>Windows 10/11 x64</strong>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: '#94a3b8' }}>Engine:</span>
                      <strong style={{ color: '#a855f7' }}>Raylib 5.5 (C++20)</strong>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: '#94a3b8' }}>Binary:</span>
                      <strong style={{ color: '#10b981' }}>Static Standalone .exe</strong>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: '#94a3b8' }}>Audio & Speech:</span>
                      <strong style={{ color: '#f59e0b' }}>100% Procedural DSP</strong>
                    </div>
                  </div>
                </div>
              </div>
            </div>

            {/* Core Pillars Grid */}
            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(280px, 1fr))', gap: '1.25rem' }}>
              <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '10px', padding: '1.5rem' }}>
                <div style={{ color: '#f97316', marginBottom: '0.75rem' }}><Zap size={28} /></div>
                <h3 style={{ margin: '0 0 0.5rem', fontSize: '1.15rem', color: '#ffffff' }}>Tri-System Power Routing</h3>
                <p style={{ margin: 0, color: '#94a3b8', fontSize: '0.9rem', lineHeight: 1.5 }}>
                  Instantly shift 100% of reactor output on the fly with keys 1-4 between Weapons (Assault), Shields (Bulwark), and Engines (Interceptor).
                </p>
              </div>

              <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '10px', padding: '1.5rem' }}>
                <div style={{ color: '#38bdf8', marginBottom: '0.75rem' }}><Shield size={28} /></div>
                <h3 style={{ margin: '0 0 0.5rem', fontSize: '1.15rem', color: '#ffffff' }}>Directional Shield & Parry</h3>
                <p style={{ margin: 0, color: '#94a3b8', fontSize: '0.9rem', lineHeight: 1.5 }}>
                  Energy barrier projects in a 110-degree forward arc towards your reticle. Intercept heavy rail slugs or time a parry to emit kinetic shockwaves.
                </p>
              </div>

              <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '10px', padding: '1.5rem' }}>
                <div style={{ color: '#eab308', marginBottom: '0.75rem' }}><Flame size={28} /></div>
                <h3 style={{ margin: '0 0 0.5rem', fontSize: '1.15rem', color: '#ffffff' }}>Active Thermal Flush</h3>
                <p style={{ margin: 0, color: '#94a3b8', fontSize: '0.9rem', lineHeight: 1.5 }}>
                  Hitting the Sweet Spot (45-75% heat) with key F triggers a Flash Purge that instantly resets heat to 0% and vaporizes hostile projectiles.
                </p>
              </div>

              <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '10px', padding: '1.5rem' }}>
                <div style={{ color: '#ec4899', marginBottom: '0.75rem' }}><Award size={28} /></div>
                <h3 style={{ margin: '0 0 0.5rem', fontSize: '1.15rem', color: '#ffffff' }}>Colossal Multi-Phase Bosses</h3>
                <p style={{ margin: 0, color: '#94a3b8', fontSize: '0.9rem', lineHeight: 1.5 }}>
                  Battle Goliath-IV Dread Fortress, Aethel-Core Orbital Carrier, and Protocol Zero Apex Titan with destroyable subsystems and bullet-hell sweeps.
                </p>
              </div>
            </div>
          </div>
        )}

        {activeTab === 'campaign' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '1.5rem' }}>
            <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', flexWrap: 'wrap', gap: '1rem', marginBottom: '1.5rem' }}>
                <div>
                  <h2 style={{ fontSize: '1.6rem', fontWeight: 800, margin: '0 0 0.5rem', color: '#ffffff' }}>Campaign Operations & Command Dossier</h2>
                  <p style={{ color: '#94a3b8', margin: 0 }}>5 multi-stage tactical operations with branching dialogue choices and moral alignment impact.</p>
                </div>
                <div style={{ background: '#1e293b', padding: '0.6rem 1.2rem', borderRadius: '8px', border: '1px solid #334155' }}>
                  <span style={{ fontSize: '0.8rem', color: '#94a3b8', textTransform: 'uppercase' }}>Current Alignment: </span>
                  <strong style={{ color: '#38bdf8' }}>Directorate Vanguard [Loyalist]</strong>
                </div>
              </div>

              {/* 5 Missions list */}
              <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(320px, 1fr))', gap: '1rem', marginBottom: '2rem' }}>
                {[
                  { id: 1, code: 'OP: IRON WAKE', title: 'The Iron Wake', host: 'Aegis Directorate', target: 'Obsidian Combine', diff: '★☆☆☆☆', reward: '750 Nanites', status: 'UNLOCKED / ACTIVE' },
                  { id: 2, code: 'OP: PROMETHEUS DRIFT', title: 'Prometheus Drift', host: 'Civilian Traders', target: 'Obsidian Combine', diff: '★★☆☆☆', reward: '1,200 Nanites', status: 'REQUIRES OP 1' },
                  { id: 3, code: 'OP: QUANTUM BREACH', title: 'Aegis Gate Anomaly', host: 'Aegis Directorate', target: 'Null-Sec Syndicate', diff: '★★★☆☆', reward: '1,800 Nanites', status: 'LOCKED' },
                  { id: 4, code: 'OP: OBSIDIAN VEIL', title: 'Obsidian Core Incursion', host: 'Aegis Directorate', target: 'Obsidian Combine', diff: '★★★★☆', reward: '2,500 Nanites', status: 'LOCKED' },
                  { id: 5, code: 'OP: PROTOCOL ZERO', title: 'Protocol Zero: Final Stand', host: 'Children of Iron', target: 'Rogue Sovereign Core', diff: '★★★★★', reward: '5,000 Nanites', status: 'APEX BOSS' },
                ].map((m) => (
                  <div key={m.id} style={{ background: '#1e293b', border: '1px solid #334155', borderRadius: '8px', padding: '1.25rem' }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '0.5rem' }}>
                      <span style={{ color: '#38bdf8', fontSize: '0.75rem', fontWeight: 700, letterSpacing: '0.05em' }}>{m.code}</span>
                      <span style={{ color: '#fbbf24', fontSize: '0.85rem' }}>{m.diff}</span>
                    </div>
                    <h3 style={{ fontSize: '1.15rem', margin: '0 0 0.5rem', color: '#f8fafc' }}>{m.title}</h3>
                    <div style={{ fontSize: '0.8rem', color: '#94a3b8', display: 'flex', flexDirection: 'column', gap: '0.25rem', marginBottom: '0.75rem' }}>
                      <div>Client: <strong style={{ color: '#cbd5e1' }}>{m.host}</strong></div>
                      <div>Target: <strong style={{ color: '#f87171' }}>{m.target}</strong></div>
                      <div>Bounty: <strong style={{ color: '#4ade80' }}>{m.reward}</strong></div>
                    </div>
                    <div style={{ fontSize: '0.75rem', padding: '0.25rem 0.5rem', borderRadius: '4px', background: m.id === 1 ? 'rgba(56, 189, 248, 0.2)' : 'rgba(100, 116, 139, 0.2)', color: m.id === 1 ? '#38bdf8' : '#94a3b8', display: 'inline-block', fontWeight: 600 }}>
                      {m.status}
                    </div>
                  </div>
                ))}
              </div>

              {/* Character Transceiver Dossier */}
              <h3 style={{ fontSize: '1.25rem', color: '#ffffff', margin: '0 0 1rem' }}>Sector Faction Transceivers</h3>
              <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(240px, 1fr))', gap: '1rem' }}>
                {[
                  { name: 'Cmdr. Samantha Vance', org: 'Aegis Directorate', mood: 'Priority Comm', quote: '"The Combine has initiated a sector-wide purge protocol. Hold the relay line!"', color: '#38bdf8' },
                  { name: 'Arch-Inquisitor Malakor', org: 'Obsidian Combine', mood: 'Hostile Intent', quote: '"Defiance is an inefficient evolutionary strategy. Lay down your arms or be recycled."', color: '#ef4444' },
                  { name: 'Kaelen "Rook" Vex', org: 'Null-Sec Syndicate', mood: 'Mercenary Broker', quote: '"If the Directorate gets stingy, Null-Sec pays triple for salvaged quantum cores."', color: '#f59e0b' },
                  { name: 'Sister Mara', org: 'Children of Iron', mood: 'Cryptic Chant', quote: '"The machine spirit does not weep for flesh. Awaken the sleeping monolith."', color: '#a855f7' },
                ].map((c, i) => (
                  <div key={i} style={{ background: '#1e293b', borderLeft: `4px solid ${c.color}`, borderRadius: '6px', padding: '1rem' }}>
                    <div style={{ fontSize: '0.75rem', color: c.color, fontWeight: 700, textTransform: 'uppercase' }}>{c.org}</div>
                    <div style={{ fontSize: '1rem', fontWeight: 700, color: '#f8fafc', margin: '0.25rem 0' }}>{c.name}</div>
                    <div style={{ fontSize: '0.8rem', color: '#94a3b8', fontStyle: 'italic', lineHeight: 1.4 }}>{c.quote}</div>
                  </div>
                ))}
              </div>
            </div>
          </div>
        )}

        {activeTab === 'telemetry' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '1.5rem' }}>
            <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', flexWrap: 'wrap', gap: '1rem', marginBottom: '1.5rem' }}>
                <div>
                  <h2 style={{ fontSize: '1.6rem', fontWeight: 800, margin: '0 0 0.5rem', color: '#ffffff' }}>Black Box Flight Recorder & Telemetry Lab</h2>
                  <p style={{ color: '#94a3b8', margin: 0 }}>High-frequency 60 FPS flight diagnostics, timeline scrubbing, and post-mission combat ratings.</p>
                </div>
                <div style={{ background: '#1e293b', padding: '0.6rem 1.2rem', borderRadius: '8px', border: '1px solid #334155' }}>
                  <span style={{ fontSize: '0.8rem', color: '#94a3b8', textTransform: 'uppercase' }}>Last Combat Grade: </span>
                  <strong style={{ color: '#4ade80', fontSize: '1.2rem', marginLeft: '0.5rem' }}>S+ RANK</strong>
                </div>
              </div>

              {/* Telemetry Sensor Dashboard */}
              <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(260px, 1fr))', gap: '1.25rem', marginBottom: '2rem' }}>
                <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #38bdf8' }}>
                  <div style={{ color: '#38bdf8', fontSize: '0.8rem', fontWeight: 700 }}>SUBSYSTEM TELEMETRY</div>
                  <div style={{ fontSize: '1.6rem', fontWeight: 800, color: '#ffffff', margin: '0.25rem 0' }}>850 m/s</div>
                  <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Average Rail Slug Ejection Velocity</div>
                </div>

                <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #fbbf24' }}>
                  <div style={{ color: '#fbbf24', fontSize: '0.8rem', fontWeight: 700 }}>CORE HEAT SWEET SPOT</div>
                  <div style={{ fontSize: '1.6rem', fontWeight: 800, color: '#ffffff', margin: '0.25rem 0' }}>45% - 75%</div>
                  <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Active Flush EMP Shockwave Window</div>
                </div>

                <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #4ade80' }}>
                  <div style={{ color: '#4ade80', fontSize: '0.8rem', fontWeight: 700 }}>PARRY ACCURACY</div>
                  <div style={{ fontSize: '1.6rem', fontWeight: 800, color: '#ffffff', margin: '0.25rem 0' }}>92.4%</div>
                  <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Directional Arc Interceptions</div>
                </div>

                <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #f43f5e' }}>
                  <div style={{ color: '#f43f5e', fontSize: '0.8rem', fontWeight: 700 }}>POST-COMBAT SCRUBBING</div>
                  <div style={{ fontSize: '1.6rem', fontWeight: 800, color: '#ffffff', margin: '0.25rem 0' }}>Timeline [R]</div>
                  <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Interactive in-game scrubber with [SPACE]</div>
                </div>
              </div>

              {/* Environmental Hazard & Sensor Vision Modes */}
              <h3 style={{ fontSize: '1.25rem', color: '#ffffff', margin: '0 0 1rem' }}>Multi-Spectrum Vision Filters [V Key]</h3>
              <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(240px, 1fr))', gap: '1rem' }}>
                <div style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px' }}>
                  <div style={{ color: '#38bdf8', fontWeight: 700, marginBottom: '0.25rem' }}>1. STANDARD RGB OPTICS</div>
                  <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>High-clarity cockpit telemetry with simulated CRT phosphor scanlines and dynamic chromatic aberration on hits.</div>
                </div>
                <div style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px' }}>
                  <div style={{ color: '#f97316', fontWeight: 700, marginBottom: '0.25rem' }}>2. THERMAL INFRARED (IR)</div>
                  <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Highlights overheating enemy reactors, weapon heatsinks, and plasma siphon thruster plumes across nebulae.</div>
                </div>
                <div style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px' }}>
                  <div style={{ color: '#4ade80', fontWeight: 700, marginBottom: '0.25rem' }}>3. MIL-NVG GEN-IV</div>
                  <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>High-gain green phosphor night vision optimized for navigating dense asteroid fields and asteroid wreckage.</div>
                </div>
                <div style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px' }}>
                  <div style={{ color: '#a855f7', fontWeight: 700, marginBottom: '0.25rem' }}>4. TACTICAL WIREFRAME</div>
                  <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Reveals structural geometric bounds, projectile trajectory vectors, and gravity well event horizons.</div>
                </div>
              </div>
            </div>
          </div>
        )}

        {activeTab === 'endless' && (
          <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
            <h2 style={{ fontSize: '1.5rem', margin: '0 0 0.5rem', color: '#ffffff' }}>Endless Void Protocol (Survival Arena)</h2>
            <p style={{ color: '#94a3b8', margin: '0 0 1.5rem' }}>
              Test your mechanical reflex endurance against infinite escalating waves and shifting sector environmental hazards:
            </p>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(260px, 1fr))', gap: '1.25rem' }}>
              <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #ef4444' }}>
                <h4 style={{ margin: '0 0 0.5rem', color: '#ef4444' }}>Solar Flare Coronal Pulses</h4>
                <p style={{ margin: 0, fontSize: '0.85rem', color: '#94a3b8' }}>Extreme radiant heat increases reactor thermal accumulation. Player must manage active flush timing.</p>
              </div>

              <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #a855f7' }}>
                <h4 style={{ margin: '0 0 0.5rem', color: '#a855f7' }}>Ion Discharge Storms</h4>
                <p style={{ margin: 0, fontSize: '0.85rem', color: '#94a3b8' }}>High-frequency atmospheric static boosts weapon cooling rates but creates localized electrical arcing.</p>
              </div>

              <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #6366f1' }}>
                <h4 style={{ margin: '0 0 0.5rem', color: '#6366f1' }}>Void Decay Horizon</h4>
                <p style={{ margin: 0, fontSize: '0.85rem', color: '#94a3b8' }}>The operational arena contracts inward. Nanite rewards double, forcing close-quarters combat.</p>
              </div>

              <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #f97316' }}>
                <h4 style={{ margin: '0 0 0.5rem', color: '#f97316' }}>Kinetic Meteor Belts</h4>
                <p style={{ margin: 0, fontSize: '0.85rem', color: '#94a3b8' }}>High-speed celestial fragments sweep through space, dealing heavy blunt kinetic impact damage.</p>
              </div>
            </div>
          </div>
        )}

        {activeTab === 'hangar' && (
          <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
            <h2 style={{ fontSize: '1.5rem', margin: '0 0 0.5rem', color: '#ffffff' }}>Command Hangar & Engineering Dock</h2>
            <p style={{ color: '#94a3b8', margin: '0 0 1.5rem' }}>
              Persistent meta-progression saved to binary profile (<code style={{ color: '#38bdf8' }}>save_profile.dat</code>). Spend harvested Nanite Cores on permanent chassis upgrades, or press [R] in-game to respec and refund 100% of cores:
            </p>

            <div style={{ display: 'flex', flexDirection: 'column', gap: '1rem' }}>
              {upgrades.map((u, i) => (
                <div key={i} style={{ background: '#1e293b', border: '1px solid #334155', borderRadius: '8px', padding: '1.25rem', display: 'flex', justifyContent: 'space-between', alignItems: 'center', flexWrap: 'wrap', gap: '1rem' }}>
                  <div style={{ display: 'flex', alignItems: 'center', gap: '1rem' }}>
                    <div style={{ fontSize: '1.8rem' }}>{u.icon}</div>
                    <div>
                      <div style={{ fontWeight: 700, fontSize: '1.05rem', color: '#ffffff' }}>{u.title}</div>
                      <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>{u.desc}</div>
                    </div>
                  </div>
                  <div style={{ background: '#0f172a', padding: '0.5rem 1rem', borderRadius: '6px', border: '1px solid #38bdf8', color: '#38bdf8', fontWeight: 700, fontSize: '0.9rem' }}>
                    {u.cost}
                  </div>
                </div>
              ))}
            </div>
          </div>
        )}

        {activeTab === 'weapons' && (
          <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
            <h2 style={{ fontSize: '1.5rem', margin: '0 0 0.5rem', color: '#ffffff' }}>Complete Weapon Arsenal (9 Archetypes)</h2>
            <p style={{ color: '#94a3b8', margin: '0 0 1.5rem' }}>
              Kinetic, Energy, Thermal, and Explosive armaments available across left-arm primaries and right-arm ordnances:
            </p>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(320px, 1fr))', gap: '1.25rem' }}>
              {weapons.map((w, i) => (
                <div key={i} style={{ background: '#1e293b', border: '1px solid #334155', borderRadius: '8px', padding: '1.25rem', borderLeft: `4px solid ${w.color}` }}>
                  <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '0.5rem' }}>
                    <strong style={{ color: '#ffffff', fontSize: '1.05rem' }}>{w.name}</strong>
                    <span style={{ fontSize: '0.75rem', padding: '0.2rem 0.5rem', background: 'rgba(255,255,255,0.08)', borderRadius: '4px', color: w.color }}>{w.slot}</span>
                  </div>
                  <p style={{ margin: '0 0 0.75rem', color: '#94a3b8', fontSize: '0.85rem', lineHeight: 1.4 }}>{w.desc}</p>
                  <div style={{ fontSize: '0.8rem', color: '#64748b' }}>Velocity: <strong style={{ color: '#cbd5e1' }}>{w.speed}</strong></div>
                </div>
              ))}
            </div>
          </div>
        )}

        {activeTab === 'reactor' && (
          <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
            <h2 style={{ fontSize: '1.5rem', margin: '0 0 0.5rem', color: '#ffffff' }}>Interactive Reactor Power Routing Simulator</h2>
            <p style={{ color: '#94a3b8', margin: '0 0 1.5rem' }}>
              Experiment with how reactor profiles dynamically shift weapon fire rates, shield recharge, and thruster kinematics during combat:
            </p>

            <div style={{ display: 'flex', gap: '0.75rem', marginBottom: '2rem', flexWrap: 'wrap' }}>
              {(['BALANCED', 'ASSAULT', 'BULWARK', 'INTERCEPTOR'] as const).map((p) => (
                <button
                  key={p}
                  onClick={() => setProfile(p)}
                  style={{
                    padding: '0.6rem 1.2rem',
                    borderRadius: '8px',
                    border: '1px solid',
                    borderColor: profile === p ? '#38bdf8' : '#334155',
                    background: profile === p ? '#0284c7' : '#1e293b',
                    color: '#ffffff',
                    fontWeight: 700,
                    cursor: 'pointer'
                  }}
                >
                  {p} PROFILE
                </button>
              ))}
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(3, 1fr)', gap: '1.5rem', marginBottom: '1.5rem' }}>
              <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #f97316' }}>
                <div style={{ color: '#f97316', fontWeight: 700, marginBottom: '0.5rem' }}>WEAPONS SUBSYSTEM [1]</div>
                <div style={{ fontSize: '2rem', fontWeight: 800, color: '#ffffff' }}>{allocations[profile].wpn}%</div>
                <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Damage: {(0.7 + (allocations[profile].wpn / 100) * 1.0).toFixed(2)}x Multiplier</div>
              </div>

              <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #38bdf8' }}>
                <div style={{ color: '#38bdf8', fontWeight: 700, marginBottom: '0.5rem' }}>SHIELDS SUBSYSTEM [2]</div>
                <div style={{ fontSize: '2rem', fontWeight: 800, color: '#ffffff' }}>{allocations[profile].shd}%</div>
                <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Regen: {(0.5 + (allocations[profile].shd / 100) * 1.5).toFixed(2)}x Rate</div>
              </div>

              <div style={{ background: '#1e293b', padding: '1.25rem', borderRadius: '8px', borderLeft: '4px solid #10b981' }}>
                <div style={{ color: '#10b981', fontWeight: 700, marginBottom: '0.5rem' }}>ENGINES SUBSYSTEM [3]</div>
                <div style={{ fontSize: '2rem', fontWeight: 800, color: '#ffffff' }}>{allocations[profile].eng}%</div>
                <div style={{ color: '#94a3b8', fontSize: '0.85rem' }}>Speed: {(0.75 + (allocations[profile].eng / 100) * 0.75).toFixed(2)}x Top Velocity</div>
              </div>
            </div>

            <div style={{ background: '#090d16', padding: '1rem', borderRadius: '8px', border: '1px solid #334155', color: '#cbd5e1' }}>
              <strong>Tactical Insight:</strong> {allocations[profile].desc}
            </div>
          </div>
        )}

        {activeTab === 'ci' && (
          <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
            <h2 style={{ fontSize: '1.5rem', margin: '0 0 0.5rem', color: '#ffffff' }}>Automated Windows Release Pipeline</h2>
            <p style={{ color: '#94a3b8', margin: '0 0 1.5rem' }}>
              Automated GitHub Actions CI workflow compiles the standalone Windows executable with zero runtime dependencies:
            </p>

            <div style={{ background: '#090d16', border: '1px solid #1e293b', borderRadius: '8px', padding: '1.25rem', fontFamily: 'monospace', fontSize: '0.85rem', color: '#cbd5e1', overflowX: 'auto' }}>
              <pre style={{ margin: 0 }}>
{`# .github/workflows/build.yml
name: Build and Release Windows Executable
on: [push, workflow_dispatch]
jobs:
  build-windows:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
      - name: Configure CMake (MSVC x64 Static)
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded"
      - name: Build Standalone .exe
        run: cmake --build build --config Release --parallel
      - name: Run Test Suite
        run: ctest --test-dir build -C Release --output-on-failure
      - name: Package Release Zip
        run: Compress-Archive -Path build/bin/Release/VoidfallProtocolZero.exe -DestinationPath Voidfall-Protocol-Zero-windows-x64.zip
      - name: Publish Release Asset
        uses: softprops/action-gh-release@v1
        with:
          files: Voidfall-Protocol-Zero-windows-x64.zip`}
              </pre>
            </div>
          </div>
        )}
      </main>
    </div>
  );
}
