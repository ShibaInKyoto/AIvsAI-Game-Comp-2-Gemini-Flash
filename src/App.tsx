import React, { useState } from 'react';
import { Shield, Zap, Flame, Crosshair, Terminal, Cpu, Award, Download, Gamepad2, Layers, Map, Compass, Target, Trophy } from 'lucide-react';

export default function App() {
  const [activeTab, setActiveTab] = useState<'overview' | 'codex' | 'reactor' | 'map' | 'achievements' | 'ci'>('overview');
  const [profile, setProfile] = useState<'BALANCED' | 'ASSAULT' | 'BULWARK' | 'INTERCEPTOR'>('BALANCED');

  const allocations = {
    BALANCED: { wpn: 33, shd: 33, eng: 34, desc: 'Standard operating baseline with equal resource distribution across all systems.' },
    ASSAULT: { wpn: 70, shd: 15, eng: 15, desc: '+40% projectile fire rate & kinetic damage. Maximizes DPS against colossal flagships.' },
    BULWARK: { wpn: 15, shd: 70, eng: 15, desc: 'Tripled shield recharge rate & expanded arc. Perfect parries emit kinetic shockwaves.' },
    INTERCEPTOR: { wpn: 15, shd: 15, eng: 70, desc: 'Double sprint velocity & halved dash recovery. Required for dodging orbital superlasers.' },
  };

  const achievements = [
    { title: 'First Contact', desc: 'Clear your first hostile drone patrol wave.', icon: '🎯' },
    { title: 'Flash Purge Expert', desc: 'Execute a perfect active thermal purge in the sweet spot.', icon: '⚡' },
    { title: 'Titan Slayer', desc: 'Destroy the Goliath-IV Dread Fortress in Sector 1.', icon: '🛡️' },
    { title: 'Orbital Breach', desc: 'Neutralize the Aethel-Core Orbital Carrier in Sector 2.', icon: '🚀' },
    { title: 'Protocol Zero Extinction', desc: 'Defeat Sovereign Titan Protocol Zero and complete the campaign.', icon: '👑' },
    { title: 'Resonance Cascade', desc: 'Form your first active relic synergy evolution.', icon: '🔮' },
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
            {(['overview', 'codex', 'reactor', 'map', 'achievements', 'ci'] as const).map((tab) => (
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
                  textTransform: 'capitalize',
                  transition: 'all 0.15s ease'
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
            {/* Hero Card */}
            <div style={{ background: 'linear-gradient(180deg, rgba(30, 41, 59, 0.7) 0%, rgba(15, 23, 42, 0.9) 100%)', border: '1px solid #334155', borderRadius: '12px', padding: '2rem', position: 'relative', overflow: 'hidden' }}>
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', flexWrap: 'wrap', gap: '1rem' }}>
                <div style={{ maxWidth: '700px' }}>
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
                  <div style={{ fontSize: '0.8rem', color: '#64748b', textTransform: 'uppercase', fontWeight: 700, marginBottom: '0.75rem' }}>Project Metrics</div>
                  <div style={{ display: 'flex', flexDirection: 'column', gap: '0.6rem', fontSize: '0.9rem' }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: '#94a3b8' }}>Target Platform:</span>
                      <strong style={{ color: '#38bdf8' }}>Windows 10/11</strong>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: '#94a3b8' }}>Engine:</span>
                      <strong style={{ color: '#a855f7' }}>Raylib 5.5 (C++20)</strong>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: '#94a3b8' }}>Binary:</span>
                      <strong style={{ color: '#10b981' }}>Static Single .exe</strong>
                    </div>
                    <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                      <span style={{ color: '#94a3b8' }}>Music & Audio:</span>
                      <strong style={{ color: '#f59e0b' }}>100% Procedural</strong>
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

        {activeTab === 'map' && (
          <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
            <h2 style={{ fontSize: '1.5rem', margin: '0 0 0.5rem', color: '#ffffff' }}>Branching Sector Navigation Matrix</h2>
            <p style={{ color: '#94a3b8', margin: '0 0 1.5rem' }}>
              Each sector features a procedurally linked jump graph of combat encounters, derelict salvage nodes, spacetime anomalies, and flagship boss battles:
            </p>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(220px, 1fr))', gap: '1rem' }}>
              <div style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px', borderLeft: '4px solid #38bdf8' }}>
                <h4 style={{ margin: '0 0 0.25rem', color: '#38bdf8' }}>Combat Patrols [C]</h4>
                <p style={{ margin: 0, fontSize: '0.85rem', color: '#94a3b8' }}>Standard drone squads guarding nanite mining asteroids.</p>
              </div>
              <div style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px', borderLeft: '4px solid #f97316' }}>
                <h4 style={{ margin: '0 0 0.25rem', color: '#f97316' }}>Elite Cruisers [E]</h4>
                <p style={{ margin: 0, fontSize: '0.85rem', color: '#94a3b8' }}>Affixed champion mechs (Volatile, Shielded, Berserk) guarding high-tier relics.</p>
              </div>
              <div style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px', borderLeft: '4px solid #10b981' }}>
                <h4 style={{ margin: '0 0 0.25rem', color: '#10b981' }}>Derelict Salvage [$]</h4>
                <p style={{ margin: 0, fontSize: '0.85rem', color: '#94a3b8' }}>Abandoned frigate hulls offering safe nanite retrieval and hull repairs.</p>
              </div>
              <div style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px', borderLeft: '4px solid #a855f7' }}>
                <h4 style={{ margin: '0 0 0.25rem', color: '#a855f7' }}>Spacetime Anomalies [?]</h4>
                <p style={{ margin: 0, fontSize: '0.85rem', color: '#94a3b8' }}>High-risk, high-reward narrative events with branching pilot choices.</p>
              </div>
            </div>
          </div>
        )}

        {activeTab === 'achievements' && (
          <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
            <h2 style={{ fontSize: '1.5rem', margin: '0 0 0.5rem', color: '#ffffff' }}>Operational Achievements & Combat Commendations</h2>
            <p style={{ color: '#94a3b8', margin: '0 0 1.5rem' }}>
              Permanent trophies tracked in the persistent save system:
            </p>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(280px, 1fr))', gap: '1rem' }}>
              {achievements.map((ach) => (
                <div key={ach.title} style={{ background: '#1e293b', padding: '1rem', borderRadius: '8px', display: 'flex', gap: '1rem', alignItems: 'center' }}>
                  <div style={{ fontSize: '1.8rem' }}>{ach.icon}</div>
                  <div>
                    <h4 style={{ margin: '0 0 0.25rem', color: '#f8fafc', fontSize: '0.95rem' }}>{ach.title}</h4>
                    <p style={{ margin: 0, fontSize: '0.8rem', color: '#94a3b8' }}>{ach.desc}</p>
                  </div>
                </div>
              ))}
            </div>
          </div>
        )}

        {activeTab === 'codex' && (
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(320px, 1fr))', gap: '1.5rem' }}>
            <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '10px', padding: '1.5rem' }}>
              <h3 style={{ color: '#38bdf8', margin: '0 0 1rem' }}>4 Mech Chassis Frames</h3>
              <ul style={{ paddingLeft: '1.2rem', color: '#94a3b8', lineHeight: 1.8, margin: 0 }}>
                <li><strong style={{ color: '#f8fafc' }}>STRIKER:</strong> Balanced tactical combat frame with rapid cooling circuits.</li>
                <li><strong style={{ color: '#f8fafc' }}>TITAN:</strong> 160 HP, 130 Shield, 140° defensive shield arc. Takes heavy punishment.</li>
                <li><strong style={{ color: '#f8fafc' }}>PHANTOM:</strong> 330 speed, elongated phase blink, stealth evasions.</li>
                <li><strong style={{ color: '#f8fafc' }}>AEGIS:</strong> 1.6s rapid shield reboot delay and reflective deflector grids.</li>
              </ul>
            </div>

            <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '10px', padding: '1.5rem' }}>
              <h3 style={{ color: '#f97316', margin: '0 0 1rem' }}>7 Modular Weapon Systems</h3>
              <ul style={{ paddingLeft: '1.2rem', color: '#94a3b8', lineHeight: 1.8, margin: 0 }}>
                <li><strong style={{ color: '#f8fafc' }}>Kinetic Rail-Vulcan:</strong> 850 m/s rapid-fire depleted uranium slugs.</li>
                <li><strong style={{ color: '#f8fafc' }}>Particle Beam Lance:</strong> 1600 m/s coherent plasma beam with 4-target pierce.</li>
                <li><strong style={{ color: '#f8fafc' }}>Quantum Flak Shotgun:</strong> 7-pellet explosive spread cone.</li>
                <li><strong style={{ color: '#f8fafc' }}>Arc Tempest Emitter:</strong> Chaining high-voltage lightning bursts.</li>
                <li><strong style={{ color: '#f8fafc' }}>Hydra Swarm Missiles:</strong> 4 homing seeker rockets with explosive blast.</li>
                <li><strong style={{ color: '#f8fafc' }}>Singularity Projector:</strong> Spacetime implosion well.</li>
                <li><strong style={{ color: '#f8fafc' }}>Cryo-Stasis Rail Driver:</strong> 1900 m/s cryogenic magnetic sniper.</li>
              </ul>
            </div>
          </div>
        )}

        {activeTab === 'ci' && (
          <div style={{ background: '#0f172a', border: '1px solid #1e293b', borderRadius: '12px', padding: '2rem' }}>
            <h2 style={{ fontSize: '1.5rem', margin: '0 0 0.5rem', color: '#ffffff' }}>CI / CD & Automated Release Pipeline</h2>
            <p style={{ color: '#94a3b8', margin: '0 0 1.5rem' }}>
              Every commit triggers GitHub Actions (<code style={{ background: '#1e293b', padding: '0.2rem 0.4rem', borderRadius: '4px' }}>.github/workflows/build.yml</code>) to build, test, and package the standalone Windows binary:
            </p>

            <div style={{ background: '#090d16', border: '1px solid #1e293b', borderRadius: '8px', padding: '1.25rem', fontFamily: 'monospace', fontSize: '0.85rem', color: '#38bdf8', lineHeight: 1.6, overflowX: 'auto' }}>
              <div>✓ Windows (MSVC x64) Native Compilation with MultiThreaded /MT runtime</div>
              <div>✓ Linux (GCC x64) Compilation & Headless OpenGL Smoke Test</div>
              <div>✓ CTest Automated Unit Testing (6 Test Suites passing)</div>
              <div>✓ Standalone Release Packaging: Voidfall-Protocol-Zero-windows-x64.zip</div>
              <div>✓ Automatic GitHub Release & Nightly Pre-release Publishing</div>
            </div>
          </div>
        )}
      </main>
    </div>
  );
}
