### This is the codebase. Full repo is at [Here](https://github.com/KiranGunathilaka/savitar_robot)

<div style="background:#ffffff; color:#111; padding:18px; border-radius:12px; box-shadow:0 2px 10px rgba(0,0,0,.06); font-family:system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif; line-height:1.6;">

  <h1 style="margin:0 0 8px;">⚡ Savitar Robot — ESP32-S3 Modular Control Stack</h1>
  <p style="margin:0 0 10px;">
    <strong>Savitar</strong> is a four-motor mobile robot with an <strong>ESP32-S3</strong> brain, color &amp; ToF sensing, a two-pair
    differential drivetrain, and on-the-fly retimed control loops. The firmware is a <strong>PlatformIO</strong> C++ codebase with clean
    separation of <em>motors</em>, <em>motion</em>, <em>sensors</em>, <em>loop scheduling</em>, and <em>telemetry</em>.
  </p>

  <div style="display:flex; gap:12px; flex-wrap:wrap; margin:8px 0 12px;">
    <div style="flex:1; min-width:260px; padding:10px 12px; border:1px solid #e5e7eb; background:#fafafa; border-radius:10px;">
      <strong>Repos</strong>
      <ul style="margin:6px 0 0 18px;">
        <li><a href="https://github.com/KiranGunathilaka/savitar_robot" target="_blank" rel="noopener" style="color:#0b5; text-decoration:none;">github.com</a></li>
        <li>Motor logs: <a href="https://github.com/KiranGunathilaka/savitar_robot_codebase/tree/main/test/motor_logs" target="_blank" rel="noopener">/test/motor_logs</a></li>
      </ul>
    </div>
    <div style="flex:1; min-width:260px; padding:10px 12px; border:1px solid #e5e7eb; background:#fafafa; border-radius:10px;">
      <strong>Video</strong>
      <div style="position:relative; padding-bottom:56.25%; height:0; overflow:hidden; border-radius:8px; border:1px solid #e5e7eb; margin-top:8px;">
        <iframe src="https://www.youtube.com/embed/TuMXFdOwjP4" title="Savitar robot compilation" style="position:absolute; inset:0; width:100%; height:100%; border:0;" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>
      </div>
    </div>
  </div>

  <!-- Architecture images -->
  <div style="display:grid; grid-template-columns:repeat(auto-fit,minmax(260px,1fr)); gap:12px; margin:10px 0 12px;">
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Software.jpg" target="_blank" rel="noopener">
      <img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Software.jpg" alt="Software architecture" loading="lazy" style="width:100%; height:auto; border:1px solid #e5e7eb; border-radius:10px;">
    </a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Hardware.jpg" target="_blank" rel="noopener">
      <img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Hardware.jpg" alt="Hardware architecture" loading="lazy" style="width:100%; height:auto; border:1px solid #e5e7eb; border-radius:10px;">
    </a>
  </div>

  <hr style="border:none; border-top:1px solid #e5e7eb; margin:12px 0;">

  <h2 style="margin:0 0 8px;">🧭 High-Level Design</h2>
  <ul style="margin:0 0 10px 18px;">
    <li><strong>Two independent closed-loop differential pairs</strong> (front &amp; back), each with its own encoder PID and <em>feed-forward</em>, unified by a higher-layer <strong>Motion</strong> controller for v/ω profiles and turning.</li>
    <li><strong>Two line-follow modes</strong> with <em>dynamic control-loop timings</em>:
      <ul style="margin:6px 0 0 18px;">
        <li><strong>FAST_MODE</strong> (black/white tracking) → control loop at <strong>FAST_TICKER</strong> <em>(currently 25&nbsp;ms)</em>.</li>
        <li><strong>SLOW_MODE</strong> (4-color discrimination) → loop at <strong>SLOW_TICKER</strong> <em>(currently 60&nbsp;ms)</em>.</li>
      </ul>
      Mode switching happens on-the-fly: the ticker retimes and the sensor sampler swaps integration windows without rebooting the stack.
    </li>
    <li><strong>ESP-NOW telemetry & tuning</strong>: logs (RGB/ToF/odometry/servo) are broadcast; updated gains/params can be injected from a paired controller.</li>
  </ul>

  <h2 style="margin:0 0 8px;">🧩 Firmware Modules</h2>
    <div style="flex:1; min-width:260px; border:1px solid #e5e7eb; border-radius:10px;">
      <div style="padding:10px 12px; background:#fafafa; border-bottom:1px solid #e5e7eb;"><strong>Motors</strong></div>
      <div style="padding:10px 12px;">
        <ul style="margin:0 0 8px 18px;">
          <li>Maintains <strong>independent PID state</strong> (FWD/ROT) for <em>front</em> and <em>back</em> pairs.</li>
          <li>Applies <strong>feed-forward</strong> models (derived from rps↔PWM calibration) per wheel; accounts for <em>battery sag</em> using measured voltage.</li>
          <li>Combines translational &amp; rotational commands → per-wheel PWM via IN1/IN2/PWM pins (polarity configurable).</li>
        </ul>
        <small style="color:#555;">KP/KD sets present for front/back, with per-wheel polarity and PWM channels.</small>
      </div>
    </div>
    <div style="flex:1; min-width:260px; border:1px solid #e5e7eb; border-radius:10px;">
      <div style="padding:10px 12px; background:#fafafa; border-bottom:1px solid #e5e7eb;"><strong>Motion</strong></div>
      <div style="padding:10px 12px;">
        <ul style="margin:0 0 8px 18px;">
          <li>Holds <strong>linear</strong> and <strong>angular</strong> motion profiles with slew/accel limits.</li>
          <li>Resets/advances profiles each tick; emits unified v/ω setpoints into <em>Motors</em>.</li>
        </ul>
      </div>
    </div>
    <div style="flex:1; min-width:260px; border:1px solid #e5e7eb; border-radius:10px;">
      <div style="padding:10px 12px; background:#fafafa; border-bottom:1px solid #e5e7eb;"><strong>Sensors</strong></div>
      <div style="padding:10px 12px;">
        <ul style="margin:0 0 8px 18px;">
          <li>RGB array via <strong>I²C multiplexer</strong> (TCS34725). Switches integration window based on mode.</li>
          <li>ToF sensors (front/left/right/center-top/center-bottom) with XSHUT pin control and custom addresses.</li>
          <li>Computes steering error using <em>SENSOR_WEIGHTS</em>; classifies <strong>white / red / blue / black / unknown</strong>.</li>
        </ul>
      </div>
    </div>
    <div style="flex:1; min-width:260px; border:1px solid #e5e7eb; border-radius:10px;">
      <div style="padding:10px 12px; background:#fafafa; border-bottom:1px solid #e5e7eb;"><strong>Systick &amp; Robot</strong></div>
      <div style="padding:10px 12px;">
        <ul style="margin:0 0 8px 18px;">
          <li><strong>Systick</strong> ticker calls encoders → sensors → motion → motors at <em>FAST_TICKER</em>/<em>SLOW_TICKER</em>.</li>
          <li><code>enableSlowMode()</code> retimes the loop and flips sensor mode dynamically.</li>
          <li><strong>Robot</strong> orchestrates the flow (e.g., barcode read, maze steps), commanding sensor modes and motion sequence.</li>
        </ul>
      </div>
    </div>
    <div style="flex:1; min-width:260px; border:1px solid #e5e7eb; border-radius:10px;">
      <div style="padding:10px 12px; background:#fafafa; border-bottom:1px solid #e5e7eb;"><strong>Reporting</strong></div>
      <div style="padding:10px 12px;">
        <ul style="margin:0 0 8px 18px;">
          <li>Packs color/ToF/odometry/servo state into ESP-NOW frames to a paired MAC.</li>
          <li>Receives updated gains &amp; parameters for in-field tuning.</li>
        </ul>
      </div>
    </div>

  <h2 style="margin:14px 0 8px;">🔧 Hardware & Config Highlights</h2>
  <div style="display:grid; grid-template-columns:repeat(auto-fit,minmax(260px,1fr)); gap:12px;">
    <div style="border:1px solid #e5e7eb; border-radius:10px; padding:10px 12px;">
      <strong>Drivetrain & Encoders</strong>
      <ul style="margin:6px 0 0 18px;">
        <li><em>4 encoders</em> (LF: 18/8, LB: 3/46, RF: 48/45, RB: 35/36), <em>PULSES_PER_ROTATION</em>=1495.</li>
        <li>Wheel Ø (front/back): 63&nbsp;mm, wheel gap: 189.5&nbsp;mm → <em>DEG_PER_MM_DIFFERENCE</em> derived.</li>
        <li>PWM channels: LF=2, RF=3, LB=4, RB=5; polarity defines forward sense per wheel.</li>
      </ul>
    </div>
    <div style="border:1px solid #e5e7eb; border-radius:10px; padding:10px 12px;">
      <strong>PID & FF</strong>
      <ul style="margin:6px 0 0 18px;">
        <li>Back: FWD KP/KD = 1.0/0.5, ROT KP/KD = 9.18/0.229.</li>
        <li>Front: FWD KP/KD = 1.0/0.5, ROT KP/KD = 9.18/0.229.</li>
        <li>Bias/limits: MIN_BIAS=13, MAX_MOTOR_PERCENT=90, dead-zone floor=5%.</li>
      </ul>
    </div>
    <div style="border:1px solid #e5e7eb; border-radius:10px; padding:10px 12px;">
      <strong>Sensing</strong>
      <ul style="margin:6px 0 0 18px;">
        <li>I²C0: SDA=19, SCL=20 · I²C1: SDA=21, SCL=47.</li>
        <li>RGB sensor (TCS34725) via mux 0x70; fast/slow integration presets.</li>
        <li>ToF XSHUT: R=11, L=13, F=12, CT=10, CB=9 · I²C addrs: 0x30–0x34.</li>
        <li>Steering gains (RGB): KP=14.0, KD=5.5; weights: {2,1,0,-1,-2}.</li>
      </ul>
    </div>
    <div style="border:1px solid #e5e7eb; border-radius:10px; padding:10px 12px;">
      <strong>System & IO</strong>
      <ul style="margin:6px 0 0 18px;">
        <li>FAST_TICKER=0.025&nbsp;s · SLOW_TICKER=0.060&nbsp;s (retimed by mode).</li>
        <li>Servos: gripper=38 (0→140°), lift=14; EM pin=41; user switch=7; LED=0.</li>
        <li>ESP-NOW peer MAC baked in (<code>broadcastAddress</code>).</li>
      </ul>
    </div>
  </div>

  <h2 style="margin:14px 0 8px;">📈 Motor Characterization & Feed-Forward</h2>
  <p style="margin:0 0 8px;">Each motor was profiled to establish rps↔PWM relationships for feed-forward terms; this reduces PID effort and keeps response consistent across loads/voltage.</p>
  <div style="display:grid; grid-template-columns:repeat(auto-fit,minmax(260px,1fr)); gap:12px;">
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Figure_1.png" target="_blank" rel="noopener">
      <img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Figure_1.png" alt="Motor functions (rps vs PWM)" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;">
    </a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/image.png" target="_blank" rel="noopener">
      <img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/image.png" alt="Inverse functions" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;">
    </a>
  </div>
  <p style="margin:6px 0 12px;">Logs & CSVs for these curves are available in <a href="https://github.com/KiranGunathilaka/savitar_robot_codebase/tree/main/test/motor_logs" target="_blank" rel="noopener">test/motor_logs</a>.</p>

  <h2 style="margin:0 0 8px;">📐 PCB & CAD</h2>
  <div style="display:grid; grid-template-columns:repeat(auto-fit,minmax(240px,1fr)); gap:12px; margin:6px 0 10px;">
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Job1-2.png" target="_blank" rel="noopener">
      <img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Job1-2.png" alt="PCB 2D" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;">
    </a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Job1-6.png" target="_blank" rel="noopener">
      <img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Job1-6.png" alt="PCB 3D" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;">
    </a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Job1-7.png" target="_blank" rel="noopener">
      <img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/Job1-7.png" alt="PCB 3D (alt)" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;">
    </a>
  </div>

  <h3 style="margin:10px 0 8px;">CAD — Current (V3) & Previous</h3>
  <div style="display:grid; grid-template-columns:repeat(auto-fit,minmax(240px,1fr)); gap:12px;">
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v3/Screenshot%20(217).png" target="_blank" rel="noopener"><img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v3/Screenshot%20(217).png" alt="V3 (1)" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;"></a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v3/Screenshot%20(218).png" target="_blank" rel="noopener"><img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v3/Screenshot%20(218).png" alt="V3 (2)" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;"></a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v3/Screenshot%20(219).png" target="_blank" rel="noopener"><img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v3/Screenshot%20(219).png" alt="V3 (3)" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;"></a>
  </div>
  <div style="display:grid; grid-template-columns:repeat(auto-fit,minmax(240px,1fr)); gap:12px; margin-top:10px;">
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v1/Screenshot%20(175).png" target="_blank" rel="noopener"><img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v1/Screenshot%20(175).png" alt="V1" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;"></a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v1/Screenshot%20(177).png" target="_blank" rel="noopener"><img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v1/Screenshot%20(177).png" alt="V1 alt" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;"></a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v2/Screenshot%20(186).png" target="_blank" rel="noopener"><img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v2/Screenshot%20(186).png" alt="V2 (1)" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;"></a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v2/Screenshot%20(189).png" target="_blank" rel="noopener"><img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v2/Screenshot%20(189).png" alt="V2 (2)" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;"></a>
    <a href="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v2/Screenshot%20(193).png" target="_blank" rel="noopener"><img src="https://raw.githubusercontent.com/KiranGunathilaka/savitar_robot/main/Images/v2/Screenshot%20(193).png" alt="V2 (3)" style="width:100%; border:1px solid #e5e7eb; border-radius:10px;"></a>
  </div>

  <h2 style="margin:14px 0 8px;">🛠️ Build & Calibrate (high-level)</h2>
  <ol style="margin:0 0 10px 18px;">
    <li>Open the repo in <strong>PlatformIO</strong> and select the ESP32-S3 environment.</li>
    <li>Wire up encoders/motors/sensors per <em>configuration header</em> (pins & addresses above).</li>
    <li>Upload, verify encoder counts, then run motor characterization if you change the drivetrain.</li>
    <li>Calibrate RGB sensors (white/black &amp; colors) and ToF offsets; save constants.</li>
    <li>Test <strong>FAST_MODE</strong> and <strong>SLOW_MODE</strong> switching; check loop timing histograms in ESP-NOW logs.</li>
  </ol>

  <h2 style="margin:0 0 8px;">🚀 Notes & Next Steps</h2>
  <ul style="margin:0 0 4px 18px;">
    <li>Add loop-overrun counters and a small timing histogram to the logger.</li>
    <li>Store per-mode PID/FF presets with bumpless transfer on retime.</li>
    <li>Extend obstacle handling (wall following & ToF fusion) with a small EKF for heading drift.</li>
  </ul>

</div>
