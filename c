<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>ProCam Studio</title>
<link href="https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/css/all.min.css">
<style>
  :root {
    --bg: #0a0a0c;
    --fg: #f0ece4;
    --muted: #6b6870;
    --accent: #e8a035;
    --accent2: #e85d35;
    --card: rgba(255,255,255,0.06);
    --border: rgba(255,255,255,0.08);
    --glass: rgba(10,10,12,0.75);
    --radius: 14px;
  }

  * { margin:0; padding:0; box-sizing:border-box; }

  body {
    font-family: 'Space Grotesk', sans-serif;
    background: var(--bg);
    color: var(--fg);
    overflow: hidden;
    height: 100vh;
    width: 100vw;
    position: relative;
  }

  /* Background atmosphere */
  .bg-glow {
    position: fixed;
    width: 500px; height: 500px;
    border-radius: 50%;
    filter: blur(150px);
    opacity: 0.15;
    pointer-events: none;
    z-index: 0;
  }
  .bg-glow.g1 { top: -200px; left: -100px; background: var(--accent); }
  .bg-glow.g2 { bottom: -200px; right: -100px; background: var(--accent2); }

  /* Main camera view */
  .camera-container {
    position: relative;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1;
  }

  #videoElement {
    width: 100%;
    height: 100%;
    object-fit: cover;
    transform: scaleX(1);
    transition: filter 0.4s ease;
  }

  #videoElement.mirror {
    transform: scaleX(-1);
  }

  /* Capture canvas (hidden) */
  #captureCanvas { display: none; }

  /* Top bar */
  .top-bar {
    position: fixed;
    top: 0; left: 0; right: 0;
    padding: 16px 20px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    z-index: 10;
    background: linear-gradient(to bottom, rgba(0,0,0,0.6), transparent);
  }

  .top-bar .brand {
    font-weight: 700;
    font-size: 18px;
    letter-spacing: 2px;
    text-transform: uppercase;
    background: linear-gradient(135deg, var(--accent), var(--accent2));
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
  }

  .top-bar .info-pill {
    font-family: 'JetBrains Mono', monospace;
    font-size: 11px;
    padding: 4px 12px;
    background: var(--card);
    border: 1px solid var(--border);
    border-radius: 20px;
    color: var(--muted);
    backdrop-filter: blur(10px);
  }

  .top-controls {
    display: flex;
    gap: 8px;
  }

  .icon-btn {
    width: 40px; height: 40px;
    border-radius: 50%;
    border: 1px solid var(--border);
    background: var(--glass);
    backdrop-filter: blur(16px);
    color: var(--fg);
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    transition: all 0.25s ease;
    font-size: 14px;
  }
  .icon-btn:hover { background: rgba(255,255,255,0.12); transform: scale(1.08); }
  .icon-btn.active { background: var(--accent); color: var(--bg); border-color: var(--accent); }

  /* Side controls */
  .side-controls {
    position: fixed;
    right: 16px;
    top: 50%;
    transform: translateY(-50%);
    display: flex;
    flex-direction: column;
    gap: 10px;
    z-index: 10;
  }

  .side-btn {
    width: 44px; height: 44px;
    border-radius: 12px;
    border: 1px solid var(--border);
    background: var(--glass);
    backdrop-filter: blur(16px);
    color: var(--fg);
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    transition: all 0.25s ease;
    font-size: 16px;
    position: relative;
  }
  .side-btn:hover { background: rgba(255,255,255,0.12); transform: scale(1.06); }
  .side-btn.active { background: var(--accent); color: var(--bg); border-color: var(--accent); }

  .side-btn .tooltip {
    position: absolute;
    right: 54px;
    white-space: nowrap;
    font-size: 11px;
    padding: 4px 10px;
    background: var(--glass);
    border: 1px solid var(--border);
    border-radius: 8px;
    backdrop-filter: blur(10px);
    opacity: 0;
    pointer-events: none;
    transition: opacity 0.2s;
  }
  .side-btn:hover .tooltip { opacity: 1; }

  /* Bottom controls */
  .bottom-bar {
    position: fixed;
    bottom: 0; left: 0; right: 0;
    padding: 20px 30px 34px;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 18px;
    z-index: 10;
    background: linear-gradient(to top, rgba(0,0,0,0.7), transparent);
  }

  /* Zoom slider */
  .zoom-row {
    display: flex;
    align-items: center;
    gap: 12px;
    width: 100%;
    max-width: 340px;
  }
  .zoom-row span {
    font-family: 'JetBrains Mono', monospace;
    font-size: 10px;
    color: var(--muted);
    min-width: 36px;
    text-align: center;
  }
  .zoom-slider {
    flex: 1;
    -webkit-appearance: none;
    appearance: none;
    height: 3px;
    border-radius: 2px;
    background: var(--border);
    outline: none;
  }
  .zoom-slider::-webkit-slider-thumb {
    -webkit-appearance: none;
    width: 16px; height: 16px;
    border-radius: 50%;
    background: var(--accent);
    cursor: pointer;
    box-shadow: 0 0 10px rgba(232,160,53,0.4);
  }

  /* Capture row */
  .capture-row {
    display: flex;
    align-items: center;
    gap: 30px;
  }

  .gallery-thumb {
    width: 50px; height: 50px;
    border-radius: 12px;
    border: 2px solid var(--border);
    overflow: hidden;
    cursor: pointer;
    transition: all 0.25s ease;
    background: var(--card);
  }
  .gallery-thumb:hover { transform: scale(1.1); border-color: var(--accent); }
  .gallery-thumb img { width: 100%; height: 100%; object-fit: cover; }

  .capture-btn {
    width: 72px; height: 72px;
    border-radius: 50%;
    border: 3px solid var(--fg);
    background: transparent;
    cursor: pointer;
    position: relative;
    transition: all 0.2s ease;
  }
  .capture-btn::after {
    content: '';
    position: absolute;
    inset: 5px;
    border-radius: 50%;
    background: var(--fg);
    transition: all 0.15s ease;
  }
  .capture-btn:hover { transform: scale(1.06); }
  .capture-btn:active { transform: scale(0.94); }
  .capture-btn:active::after { background: var(--accent); }

  .capture-btn.recording { border-color: #e83535; }
  .capture-btn.recording::after {
    background: #e83535;
    animation: pulse-rec 1s ease infinite;
  }

  @keyframes pulse-rec {
    0%, 100% { transform: scale(1); }
    50% { transform: scale(0.85); }
  }

  .switch-btn {
    width: 50px; height: 50px;
    border-radius: 50%;
    border: 1px solid var(--border);
    background: var(--glass);
    backdrop-filter: blur(16px);
    color: var(--fg);
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    transition: all 0.3s ease;
    font-size: 18px;
  }
  .switch-btn:hover { background: rgba(255,255,255,0.12); transform: rotate(180deg); }

  /* Filter strip */
  .filter-strip {
    display: flex;
    gap: 10px;
    overflow-x: auto;
    max-width: 100%;
    padding: 4px 0;
    scrollbar-width: none;
  }
  .filter-strip::-webkit-scrollbar { display: none; }

  .filter-chip {
    flex-shrink: 0;
    padding: 6px 16px;
    border-radius: 20px;
    border: 1px solid var(--border);
    background: var(--glass);
    backdrop-filter: blur(10px);
    font-size: 12px;
    font-weight: 500;
    cursor: pointer;
    transition: all 0.25s ease;
    color: var(--muted);
  }
  .filter-chip:hover { color: var(--fg); border-color: rgba(255,255,255,0.2); }
  .filter-chip.active {
    background: var(--accent);
    color: var(--bg);
    border-color: var(--accent);
    font-weight: 600;
  }

  /* Timer overlay */
  .timer-overlay {
    position: fixed;
    inset: 0;
    display: none;
    align-items: center;
    justify-content: center;
    z-index: 20;
    pointer-events: none;
  }
  .timer-overlay.show { display: flex; }
  .timer-overlay .timer-num {
    font-size: 140px;
    font-weight: 700;
    color: var(--fg);
    text-shadow: 0 0 80px rgba(232,160,53,0.5);
    animation: timer-pop 0.6s ease;
  }
  @keyframes timer-pop {
    0% { transform: scale(2); opacity: 0; }
    50% { transform: scale(0.9); opacity: 1; }
    100% { transform: scale(1); opacity: 1; }
  }

  /* Flash overlay */
  .flash-overlay {
    position: fixed;
    inset: 0;
    background: white;
    opacity: 0;
    pointer-events: none;
    z-index: 30;
    transition: opacity 0.08s ease;
  }
  .flash-overlay.flash { opacity: 0.7; }

  /* Gallery panel */
  .gallery-panel {
    position: fixed;
    inset: 0;
    background: var(--bg);
    z-index: 50;
    display: none;
    flex-direction: column;
    animation: slideUp 0.35s ease;
  }
  .gallery-panel.open { display: flex; }

  @keyframes slideUp {
    from { transform: translateY(100%); }
    to { transform: translateY(0); }
  }

  .gallery-header {
    padding: 16px 20px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    border-bottom: 1px solid var(--border);
  }
  .gallery-header h2 {
    font-size: 20px;
    font-weight: 600;
  }
  .gallery-close {
    width: 36px; height: 36px;
    border-radius: 50%;
    border: 1px solid var(--border);
    background: transparent;
    color: var(--fg);
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 16px;
    transition: all 0.2s;
  }
  .gallery-close:hover { background: rgba(255,255,255,0.1); }

  .gallery-grid {
    flex: 1;
    overflow-y: auto;
    padding: 16px;
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
    gap: 10px;
  }
  .gallery-item {
    aspect-ratio: 1;
    border-radius: var(--radius);
    overflow: hidden;
    cursor: pointer;
    position: relative;
    transition: transform 0.2s;
  }
  .gallery-item:hover { transform: scale(1.03); }
  .gallery-item img { width: 100%; height: 100%; object-fit: cover; }
  .gallery-item .item-actions {
    position: absolute;
    bottom: 0; left: 0; right: 0;
    padding: 8px;
    display: flex;
    gap: 6px;
    justify-content: flex-end;
    background: linear-gradient(transparent, rgba(0,0,0,0.7));
    opacity: 0;
    transition: opacity 0.2s;
  }
  .gallery-item:hover .item-actions { opacity: 1; }
  .item-action-btn {
    width: 30px; height: 30px;
    border-radius: 8px;
    border: none;
    background: rgba(255,255,255,0.15);
    backdrop-filter: blur(6px);
    color: white;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 12px;
    transition: background 0.2s;
  }
  .item-action-btn:hover { background: var(--accent); color: var(--bg); }

  /* Preview modal */
  .preview-modal {
    position: fixed;
    inset: 0;
    background: rgba(0,0,0,0.92);
    backdrop-filter: blur(20px);
    z-index: 60;
    display: none;
    align-items: center;
    justify-content: center;
    flex-direction: column;
    gap: 20px;
  }
  .preview-modal.open { display: flex; }
  .preview-modal img {
    max-width: 90vw;
    max-height: 75vh;
    border-radius: var(--radius);
    object-fit: contain;
  }
  .preview-actions {
    display: flex;
    gap: 12px;
  }
  .preview-btn {
    padding: 10px 24px;
    border-radius: 10px;
    border: 1px solid var(--border);
    background: var(--glass);
    color: var(--fg);
    font-family: 'Space Grotesk', sans-serif;
    font-size: 14px;
    cursor: pointer;
    transition: all 0.2s;
    display: flex;
    align-items: center;
    gap: 8px;
  }
  .preview-btn:hover { background: rgba(255,255,255,0.12); }
  .preview-btn.primary { background: var(--accent); color: var(--bg); border-color: var(--accent); }
  .preview-btn.primary:hover { background: #d4902e; }

  /* Toast */
  .toast-container {
    position: fixed;
    top: 80px;
    left: 50%;
    transform: translateX(-50%);
    z-index: 100;
    display: flex;
    flex-direction: column;
    gap: 8px;
    align-items: center;
  }
  .toast {
    padding: 10px 20px;
    border-radius: 10px;
    background: var(--glass);
    backdrop-filter: blur(16px);
    border: 1px solid var(--border);
    font-size: 13px;
    color: var(--fg);
    animation: toastIn 0.3s ease, toastOut 0.3s ease 2.2s forwards;
    display: flex;
    align-items: center;
    gap: 8px;
  }
  .toast i { color: var(--accent); }
  @keyframes toastIn { from { opacity:0; transform: translateY(-10px); } to { opacity:1; transform: translateY(0); } }
  @keyframes toastOut { to { opacity:0; transform: translateY(-10px); } }

  /* Settings panel */
  .settings-panel {
    position: fixed;
    right: -320px;
    top: 0; bottom: 0;
    width: 300px;
    background: var(--bg);
    border-left: 1px solid var(--border);
    z-index: 40;
    padding: 24px 20px;
    overflow-y: auto;
    transition: right 0.35s ease;
  }
  .settings-panel.open { right: 0; }

  .settings-title {
    font-size: 18px;
    font-weight: 600;
    margin-bottom: 20px;
    display: flex;
    justify-content: space-between;
    align-items: center;
  }

  .setting-group {
    margin-bottom: 20px;
  }
  .setting-group label {
    display: block;
    font-size: 12px;
    color: var(--muted);
    margin-bottom: 8px;
    text-transform: uppercase;
    letter-spacing: 1px;
  }
  .setting-group select, .setting-group input[type="number"] {
    width: 100%;
    padding: 10px 14px;
    border-radius: 10px;
    border: 1px solid var(--border);
    background: var(--card);
    color: var(--fg);
    font-family: 'JetBrains Mono', monospace;
    font-size: 13px;
    outline: none;
    transition: border-color 0.2s;
  }
  .setting-group select:focus, .setting-group input:focus {
    border-color: var(--accent);
  }
  .setting-group select option { background: var(--bg); }

  .toggle-row {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 10px 0;
    border-bottom: 1px solid var(--border);
  }
  .toggle-row span { font-size: 14px; }
  .toggle-switch {
    width: 44px; height: 24px;
    border-radius: 12px;
    background: var(--card);
    border: 1px solid var(--border);
    cursor: pointer;
    position: relative;
    transition: all 0.25s;
  }
  .toggle-switch::after {
    content: '';
    position: absolute;
    width: 18px; height: 18px;
    border-radius: 50%;
    background: var(--muted);
    top: 2px; left: 2px;
    transition: all 0.25s;
  }
  .toggle-switch.on { background: var(--accent); border-color: var(--accent); }
  .toggle-switch.on::after { left: 22px; background: var(--bg); }

  /* No camera fallback */
  .no-camera {
    display: none;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 16px;
    text-align: center;
    padding: 40px;
  }
  .no-camera i { font-size: 60px; color: var(--muted); }
  .no-camera h2 { font-size: 22px; }
  .no-camera p { color: var(--muted); font-size: 14px; max-width: 300px; }

  /* Grid overlay */
  .grid-overlay {
    position: fixed;
    inset: 0;
    pointer-events: none;
    z-index: 5;
    display: none;
  }
  .grid-overlay.show { display: block; }
  .grid-overlay svg { width: 100%; height: 100%; }
  .grid-line { stroke: rgba(255,255,255,0.15); stroke-width: 1; }

  /* Level indicator */
  .level-indicator {
    position: fixed;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    pointer-events: none;
    z-index: 5;
    display: none;
  }
  .level-indicator.show { display: block; }
  .crosshair {
    width: 60px; height: 60px;
    border: 1px solid rgba(232,160,53,0.4);
    border-radius: 50%;
    position: relative;
  }
  .crosshair::before, .crosshair::after {
    content: '';
    position: absolute;
    background: rgba(232,160,53,0.5);
  }
  .crosshair::before { width: 1px; height: 20px; top: 50%; left: 50%; transform: translate(-50%, -50%); }
  .crosshair::after { width: 20px; height: 1px; top: 50%; left: 50%; transform: translate(-50%, -50%); }

  /* Responsive */
  @media (max-width: 600px) {
    .side-controls { right: 8px; }
    .side-btn { width: 38px; height: 38px; font-size: 14px; }
    .side-btn .tooltip { display: none; }
    .capture-btn { width: 62px; height: 62px; }
    .bottom-bar { padding: 14px 16px 28px; }
    .filter-chip { padding: 5px 12px; font-size: 11px; }
    .settings-panel { width: 260px; }
  }

  @media (prefers-reduced-motion: reduce) {
    *, *::before, *::after {
      animation-duration: 0.01ms !important;
      transition-duration: 0.01ms !important;
    }
  }
</style>
</head>
<body>

<div class="bg-glow g1"></div>
<div class="bg-glow g2"></div>

<!-- Flash overlay -->
<div class="flash-overlay" id="flashOverlay"></div>

<!-- Timer overlay -->
<div class="timer-overlay" id="timerOverlay">
  <span class="timer-num" id="timerNum"></span>
</div>

<!-- Grid overlay -->
<div class="grid-overlay" id="gridOverlay">
  <svg viewBox="0 0 100 100" preserveAspectRatio="none">
    <line class="grid-line" x1="33.33" y1="0" x2="33.33" y2="100"/>
    <line class="grid-line" x1="66.66" y1="0" x2="66.66" y2="100"/>
    <line class="grid-line" x1="0" y1="33.33" x2="100" y2="33.33"/>
    <line class="grid-line" x1="0" y1="66.66" x2="100" y2="66.66"/>
  </svg>
</div>

<!-- Level / Crosshair -->
<div class="level-indicator" id="levelIndicator">
  <div class="crosshair"></div>
</div>

<!-- Top bar -->
<div class="top-bar">
  <div class="brand">ProCam</div>
  <div class="info-pill" id="resInfo">--</div>
  <div class="top-controls">
    <button class="icon-btn" id="gridBtn" title="Grid" aria-label="Toggle grid overlay">
      <i class="fas fa-border-all"></i>
    </button>
    <button class="icon-btn" id="levelBtn" title="Level" aria-label="Toggle level indicator">
      <i class="fas fa-crosshairs"></i>
    </button>
    <button class="icon-btn" id="settingsBtn" title="Settings" aria-label="Open settings">
      <i class="fas fa-sliders"></i>
    </button>
  </div>
</div>

<!-- Side controls -->
<div class="side-controls">
  <button class="side-btn" id="flashToggle" aria-label="Toggle flash">
    <i class="fas fa-bolt"></i>
    <span class="tooltip">Flash</span>
  </button>
  <button class="side-btn" id="timerBtn" aria-label="Timer">
    <i class="fas fa-clock"></i>
    <span class="tooltip">Timer</span>
  </button>
  <button class="side-btn" id="mirrorBtn" aria-label="Mirror">
    <i class="fas fa-right-left"></i>
    <span class="tooltip">Mirror</span>
  </button>
  <button class="side-btn" id="ratioBtn" aria-label="Aspect ratio">
    <i class="fas fa-expand"></i>
    <span class="tooltip">Ratio</span>
  </button>
</div>

<!-- Camera view -->
<div class="camera-container" id="cameraContainer">
  <video id="videoElement" autoplay playsinline muted></video>
  <canvas id="captureCanvas"></canvas>
  <div class="no-camera" id="noCamera">
    <i class="fas fa-video-slash"></i>
    <h2>Camera Not Available</h2>
    <p>Please allow camera access in your browser settings and reload the page.</p>
  </div>
</div>

<!-- Bottom bar -->
<div class="bottom-bar">
  <div class="filter-strip" id="filterStrip">
    <div class="filter-chip active" data-filter="none">Normal</div>
    <div class="filter-chip" data-filter="grayscale(100%)">Mono</div>
    <div class="filter-chip" data-filter="sepia(80%)">Sepia</div>
    <div class="filter-chip" data-filter="contrast(140%) saturate(130%)">Vivid</div>
    <div class="filter-chip" data-filter="brightness(110%) contrast(90%)">Soft</div>
    <div class="filter-chip" data-filter="contrast(150%)">Sharp</div>
    <div class="filter-chip" data-filter="saturate(200%)">Saturate</div>
    <div class="filter-chip" data-filter="hue-rotate(90deg)">Cool</div>
    <div class="filter-chip" data-filter="hue-rotate(180deg)">Invert Hue</div>
    <div class="filter-chip" data-filter="brightness(120%) hue-rotate(-20deg) saturate(80%)">Warm</div>
    <div class="filter-chip" data-filter="contrast(120%) brightness(90%) saturate(80%)">Film</div>
    <div class="filter-chip" data-filter="grayscale(50%) contrast(130%)">Noir</div>
  </div>

  <div class="zoom-row">
    <span id="zoomLabel">1.0x</span>
    <input type="range" class="zoom-slider" id="zoomSlider" min="1" max="5" step="0.1" value="1" aria-label="Zoom">
    <span>5.0x</span>
  </div>

  <div class="capture-row">
    <div class="gallery-thumb" id="galleryThumb" aria-label="Open gallery">
      <i class="fas fa-images" style="display:flex;align-items:center;justify-content:center;height:100%;color:var(--muted);font-size:18px;"></i>
    </div>
    <button class="capture-btn" id="captureBtn" aria-label="Capture photo"></button>
    <button class="switch-btn" id="switchBtn" aria-label="Switch camera">
      <i class="fas fa-camera-rotate"></i>
    </button>
  </div>
</div>

<!-- Settings panel -->
<div class="settings-panel" id="settingsPanel">
  <div class="settings-title">
    Settings
    <button class="gallery-close" id="closeSettings" aria-label="Close settings"><i class="fas fa-xmark"></i></button>
  </div>

  <div class="setting-group">
    <label>Resolution</label>
    <select id="resSelect">
      <option value="3840x2160">4K Ultra HD (3840x2160)</option>
      <option value="1920x1080" selected>Full HD (1920x1080)</option>
      <option value="1280x720">HD (1280x720)</option>
      <option value="640x480">SD (640x480)</option>
    </select>
  </div>

  <div class="setting-group">
    <label>Frame Rate</label>
    <select id="fpsSelect">
      <option value="60">60 fps</option>
      <option value="30" selected>30 fps</option>
      <option value="24">24 fps (Cinema)</option>
    </select>
  </div>

  <div class="setting-group">
    <label>Capture Format</label>
    <select id="formatSelect">
      <option value="image/png">PNG (Lossless)</option>
      <option value="image/jpeg" selected>JPEG (Compressed)</option>
      <option value="image/webp">WebP (Modern)</option>
    </select>
  </div>

  <div class="setting-group">
    <label>JPEG / WebP Quality</label>
    <input type="number" id="qualityInput" min="0.1" max="1" step="0.1" value="0.95">
  </div>

  <div class="toggle-row">
    <span>Flash on Capture</span>
    <div class="toggle-switch" id="flashCaptureToggle" role="switch" aria-label="Flash on capture"></div>
  </div>
  <div class="toggle-row">
    <span>Sound Effect</span>
    <div class="toggle-switch on" id="soundToggle" role="switch" aria-label="Sound effect"></div>
  </div>
  <div class="toggle-row">
    <span>Auto Mirror (Front)</span>
    <div class="toggle-switch on" id="autoMirrorToggle" role="switch" aria-label="Auto mirror front camera"></div>
  </div>
</div>

<!-- Gallery panel -->
<div class="gallery-panel" id="galleryPanel">
  <div class="gallery-header">
    <h2>Gallery</h2>
    <button class="gallery-close" id="closeGallery" aria-label="Close gallery"><i class="fas fa-xmark"></i></button>
  </div>
  <div class="gallery-grid" id="galleryGrid"></div>
</div>

<!-- Preview modal -->
<div class="preview-modal" id="previewModal">
  <img id="previewImg" src="" alt="Photo preview">
  <div class="preview-actions">
    <button class="preview-btn" id="previewClose"><i class="fas fa-xmark"></i> Close</button>
    <button class="preview-btn primary" id="previewDownload"><i class="fas fa-download"></i> Download</button>
  </div>
</div>

<!-- Toast container -->
<div class="toast-container" id="toastContainer"></div>

<script>
  // ========== State ==========
  const state = {
    stream: null,
    facingMode: 'environment',
    currentFilter: 'none',
    zoom: 1,
    timerSeconds: 0, // 0 = off, 3, 5, 10
    flashOn: false,
    gridOn: false,
    levelOn: false,
    mirror: false,
    autoMirror: true,
    flashOnCapture: true,
    soundOn: true,
    photos: [],
    ratio: '16:9', // 16:9, 4:3, 1:1
    ratios: ['16:9', '4:3', '1:1'],
    ratioIndex: 0,
  };

  // ========== DOM Elements ==========
  const video = document.getElementById('videoElement');
  const canvas = document.getElementById('captureCanvas');
  const ctx = canvas.getContext('2d');
  const flashOverlay = document.getElementById('flashOverlay');
  const timerOverlay = document.getElementById('timerOverlay');
  const timerNum = document.getElementById('timerNum');
  const gridOverlay = document.getElementById('gridOverlay');
  const levelIndicator = document.getElementById('levelIndicator');
  const noCamera = document.getElementById('noCamera');
  const resInfo = document.getElementById('resInfo');
  const zoomSlider = document.getElementById('zoomSlider');
  const zoomLabel = document.getElementById('zoomLabel');
  const captureBtn = document.getElementById('captureBtn');
  const switchBtn = document.getElementById('switchBtn');
  const galleryThumb = document.getElementById('galleryThumb');
  const galleryPanel = document.getElementById('galleryPanel');
  const galleryGrid = document.getElementById('galleryGrid');
  const previewModal = document.getElementById('previewModal');
  const previewImg = document.getElementById('previewImg');
  const settingsPanel = document.getElementById('settingsPanel');
  const filterStrip = document.getElementById('filterStrip');
  const toastContainer = document.getElementById('toastContainer');

  // ========== Toast ==========
  function showToast(message, icon = 'fa-circle-check') {
    const toast = document.createElement('div');
    toast.className = 'toast';
    toast.innerHTML = `<i class="fas ${icon}"></i> ${message}`;
    toastContainer.appendChild(toast);
    setTimeout(() => toast.remove(), 2600);
  }

  // ========== Camera Init ==========
  async function startCamera() {
    if (state.stream) {
      state.stream.getTracks().forEach(t => t.stop());
    }

    const res = document.getElementById('resSelect').value.split('x');
    const fps = parseInt(document.getElementById('fpsSelect').value);

    const constraints = {
      video: {
        facingMode: state.facingMode,
        width: { ideal: parseInt(res[0]) },
        height: { ideal: parseInt(res[1]) },
        frameRate: { ideal: fps }
      },
      audio: false
    };

    try {
      state.stream = await navigator.mediaDevices.getUserMedia(constraints);
      video.srcObject = state.stream;

      video.onloadedmetadata = () => {
        const vw = video.videoWidth;
        const vh = video.videoHeight;
        resInfo.textContent = `${vw}x${vh} @ ${fps}fps`;

        // Auto-mirror front camera
        if (state.autoMirror && state.facingMode === 'user') {
          video.classList.add('mirror');
          state.mirror = true;
        } else if (!state.autoMirror) {
          video.classList.remove('mirror');
          state.mirror = false;
        }

        // Apply zoom support if available
        const track = state.stream.getVideoTracks()[0];
        const capabilities = track.getCapabilities ? track.getCapabilities() : {};
        if (capabilities.zoom) {
          zoomSlider.min = capabilities.zoom.min;
          zoomSlider.max = capabilities.zoom.max;
          zoomSlider.step = capabilities.zoom.step || 0.1;
        }
      };

      noCamera.style.display = 'none';
      video.style.display = 'block';

      // Flash torch support
      updateFlashSupport();

    } catch (err) {
      console.error('Camera error:', err);
      noCamera.style.display = 'flex';
      video.style.display = 'none';
      showToast('Camera access denied', 'fa-triangle-exclamation');
    }
  }

  function updateFlashSupport() {
    if (!state.stream) return;
    const track = state.stream.getVideoTracks()[0];
    const capabilities = track.getCapabilities ? track.getCapabilities() : {};
    const flashBtn = document.getElementById('flashToggle');
    if (!capabilities.torch) {
      flashBtn.style.opacity = '0.4';
      flashBtn.style.pointerEvents = 'none';
    } else {
      flashBtn.style.opacity = '1';
      flashBtn.style.pointerEvents = 'auto';
    }
  }

  // ========== Zoom ==========
  zoomSlider.addEventListener('input', () => {
    const val = parseFloat(zoomSlider.value);
    state.zoom = val;
    zoomLabel.textContent = val.toFixed(1) + 'x';

    // Try hardware zoom first
    if (state.stream) {
      const track = state.stream.getVideoTracks()[0];
      const capabilities = track.getCapabilities ? track.getCapabilities() : {};
      if (capabilities.zoom) {
        track.applyConstraints({ advanced: [{ zoom: Math.min(val, capabilities.zoom.max) }] }).catch(() => {});
      }
    }

    // Software zoom fallback
    video.style.transform = `${state.mirror ? 'scaleX(-1)' : ''} scale(${val})`;
  });

  // ========== Switch Camera ==========
  switchBtn.addEventListener('click', () => {
    state.facingMode = state.facingMode === 'environment' ? 'user' : 'environment';
    startCamera();
    showToast(state.facingMode === 'user' ? 'Front camera' : 'Rear camera', 'fa-camera-rotate');
  });

  // ========== Flash ==========
  document.getElementById('flashToggle').addEventListener('click', function() {
    state.flashOn = !state.flashOn;
    this.classList.toggle('active', state.flashOn);

    if (state.stream) {
      const track = state.stream.getVideoTracks()[0];
      track.applyConstraints({ advanced: [{ torch: state.flashOn }] }).catch(() => {
        showToast('Flash not supported on this device', 'fa-triangle-exclamation');
      });
    }
  });

  // ========== Timer ==========
  document.getElementById('timerBtn').addEventListener('click', function() {
    const options = [0, 3, 5, 10];
    const idx = options.indexOf(state.timerSeconds);
    state.timerSeconds = options[(idx + 1) % options.length];
    this.classList.toggle('active', state.timerSeconds > 0);

    const labels = { 0: 'Off', 3: '3s', 5: '5s', 10: '10s' };
    showToast(`Timer: ${labels[state.timerSeconds]}`, 'fa-clock');
  });

  // ========== Mirror ==========
  document.getElementById('mirrorBtn').addEventListener('click', function() {
    state.mirror = !state.mirror;
    video.classList.toggle('mirror', state.mirror);
    this.classList.toggle('active', state.mirror);
  });

  // ========== Aspect Ratio ==========
  document.getElementById('ratioBtn').addEventListener('click', function() {
    state.ratioIndex = (state.ratioIndex + 1) % state.ratios.length;
    state.ratio = state.ratios[state.ratioIndex];
    this.classList.toggle('active', state.ratio !== '16:9');

    // Visual crop simulation
    const container = document.getElementById('cameraContainer');
    if (state.ratio === '1:1') {
      container.style.padding = '0 25%';
    } else if (state.ratio === '4:3') {
      container.style.padding = '0 12%';
    } else {
      container.style.padding = '0';
    }
    showToast(`Ratio: ${state.ratio}`, 'fa-expand');
  });

  // ========== Grid ==========
  document.getElementById('gridBtn').addEventListener('click', function() {
    state.gridOn = !state.gridOn;
    gridOverlay.classList.toggle('show', state.gridOn);
    this.classList.toggle('active', state.gridOn);
  });

  // ========== Level ==========
  document.getElementById('levelBtn').addEventListener('click', function() {
    state.levelOn = !state.levelOn;
    levelIndicator.classList.toggle('show', state.levelOn);
    this.classList.toggle('active', state.levelOn);
  });

  // ========== Filters ==========
  filterStrip.addEventListener('click', (e) => {
    const chip = e.target.closest('.filter-chip');
    if (!chip) return;

    filterStrip.querySelectorAll('.filter-chip').forEach(c => c.classList.remove('active'));
    chip.classList.add('active');

    state.currentFilter = chip.dataset.filter;
    video.style.filter = state.currentFilter === 'none' ? '' : state.currentFilter;
  });

  // ========== Capture ==========
  captureBtn.addEventListener('click', async () => {
    if (state.timerSeconds > 0) {
      await runTimer(state.timerSeconds);
    }
    capturePhoto();
  });

  function runTimer(seconds) {
    return new Promise(resolve => {
      timerOverlay.classList.add('show');
      let count = seconds;
      timerNum.textContent = count;

      const interval = setInterval(() => {
        count--;
        if (count <= 0) {
          clearInterval(interval);
          timerOverlay.classList.remove('show');
          resolve();
        } else {
          timerNum.textContent = count;
          // Re-trigger animation
          timerNum.style.animation = 'none';
          timerNum.offsetHeight;
          timerNum.style.animation = 'timer-pop 0.6s ease';
        }
      }, 1000);
    });
  }

  function capturePhoto() {
    const vw = video.videoWidth;
    const vh = video.videoHeight;

    if (!vw || !vh) return;

    // Calculate crop based on ratio
    let cropW = vw, cropH = vh;
    if (state.ratio === '1:1') {
      cropW = cropH = Math.min(vw, vh);
    } else if (state.ratio === '4:3') {
      const targetRatio = 4 / 3;
      if (vw / vh > targetRatio) {
        cropW = vh * targetRatio;
      } else {
        cropH = vw / targetRatio;
      }
    }

    const offsetX = (vw - cropW) / 2;
    const offsetY = (vh - cropH) / 2;

    canvas.width = cropW;
    canvas.height = cropH;

    // Apply filter
    if (state.currentFilter !== 'none') {
      ctx.filter = state.currentFilter;
    }

    // Mirror if needed
    if (state.mirror) {
      ctx.translate(cropW, 0);
      ctx.scale(-1, 1);
    }

    ctx.drawImage(video, offsetX, offsetY, cropW, cropH, 0, 0, cropW, cropH);

    // Reset transforms
    ctx.setTransform(1, 0, 0, 1, 0, 0);
    ctx.filter = 'none';

    // Get image data
    const format = document.getElementById('formatSelect').value;
    const quality = parseFloat(document.getElementById('qualityInput').value);
    const dataUrl = canvas.toDataURL(format, quality);

    // Store photo
    const photo = {
      dataUrl,
      timestamp: Date.now(),
      width: cropW,
      height: cropH,
      format
    };
    state.photos.unshift(photo);

    // Update gallery thumb
    updateGalleryThumb();

    // Flash effect
    if (state.flashOnCapture) {
      flashOverlay.classList.add('flash');
      setTimeout(() => flashOverlay.classList.remove('flash'), 120);
    }

    // Shutter sound
    if (state.soundOn) {
      playShutterSound();
    }

    // Capture button animation
    captureBtn.style.transform = 'scale(0.9)';
    setTimeout(() => captureBtn.style.transform = '', 150);

    showToast('Photo captured', 'fa-camera');
  }

  function playShutterSound() {
    try {
      const audioCtx = new (window.AudioContext || window.webkitAudioContext)();
      const oscillator = audioCtx.createOscillator();
      const gainNode = audioCtx.createGain();

      oscillator.connect(gainNode);
      gainNode.connect(audioCtx.destination);

      oscillator.type = 'sine';
      oscillator.frequency.setValueAtTime(1200, audioCtx.currentTime);
      oscillator.frequency.exponentialRampToValueAtTime(600, audioCtx.currentTime + 0.08);

      gainNode.gain.setValueAtTime(0.3, audioCtx.currentTime);
      gainNode.gain.exponentialRampToValueAtTime(0.001, audioCtx.currentTime + 0.1);

      oscillator.start(audioCtx.currentTime);
      oscillator.stop(audioCtx.currentTime + 0.1);
    } catch (e) { /* Audio not supported */ }
  }

  function updateGalleryThumb() {
    if (state.photos.length > 0) {
      galleryThumb.innerHTML = `<img src="${state.photos[0].dataUrl}" alt="Last photo">`;
    }
  }

  // ========== Gallery ==========
  galleryThumb.addEventListener('click', openGallery);

  function openGallery() {
    galleryGrid.innerHTML = '';

    if (state.photos.length === 0) {
      galleryGrid.innerHTML = `
        <div style="grid-column: 1/-1; text-align:center; padding:60px 20px; color:var(--muted);">
          <i class="fas fa-images" style="font-size:40px; margin-bottom:12px; display:block;"></i>
          <p>No photos yet. Capture some moments.</p>
        </div>`;
    } else {
      state.photos.forEach((photo, index) => {
        const item = document.createElement('div');
        item.className = 'gallery-item';
        item.innerHTML = `
          <img src="${photo.dataUrl}" alt="Photo ${index + 1}">
          <div class="item-actions">
            <button class="item-action-btn" data-action="download" data-index="${index}" title="Download">
              <i class="fas fa-download"></i>
            </button>
            <button class="item-action-btn" data-action="delete" data-index="${index}" title="Delete">
              <i class="fas fa-trash"></i>
            </button>
          </div>`;
        item.querySelector('img').addEventListener('click', () => openPreview(index));
        galleryGrid.appendChild(item);
      });
    }

    // Bind action buttons
    galleryGrid.querySelectorAll('.item-action-btn').forEach(btn => {
      btn.addEventListener('click', (e) => {
        e.stopPropagation();
        const action = btn.dataset.action;
        const idx = parseInt(btn.dataset.index);
        if (action === 'download') downloadPhoto(idx);
        if (action === 'delete') deletePhoto(idx);
      });
    });

    galleryPanel.classList.add('open');
  }

  document.getElementById('closeGallery').addEventListener('click', () => {
    galleryPanel.classList.remove('open');
  });

  function openPreview(index) {
    previewImg.src = state.photos[index].dataUrl;
    previewModal.classList.add('open');
    previewModal.dataset.index = index;
  }

  document.getElementById('previewClose').addEventListener('click', () => {
    previewModal.classList.remove('open');
  });

  document.getElementById('previewDownload').addEventListener('click', () => {
    const idx = parseInt(previewModal.dataset.index);
    downloadPhoto(idx);
  });

  previewModal.addEventListener('click', (e) => {
    if (e.target === previewModal) previewModal.classList.remove('open');
  });

  function downloadPhoto(index) {
    const photo = state.photos[index];
    const link = document.createElement('a');
    const ext = photo.format.split('/')[1];
    link.download = `procam_${photo.timestamp}.${ext}`;
    link.href = photo.dataUrl;
    link.click();
    showToast('Photo downloaded', 'fa-download');
  }

  function deletePhoto(index) {
    state.photos.splice(index, 1);
    updateGalleryThumb();
    openGallery(); // Re-render
    showToast('Photo deleted', 'fa-trash');
  }

  // ========== Settings ==========
  document.getElementById('settingsBtn').addEventListener('click', () => {
    settingsPanel.classList.add('open');
  });

  document.getElementById('closeSettings').addEventListener('click', () => {
    settingsPanel.classList.remove('open');
  });

  // Restart camera on resolution/fps change
  document.getElementById('resSelect').addEventListener('change', startCamera);
  document.getElementById('fpsSelect').addEventListener('change', startCamera);

  // Toggles
  function setupToggle(id, key) {
    const el = document.getElementById(id);
    el.addEventListener('click', () => {
      state[key] = !state[key];
      el.classList.toggle('on', state[key]);
    });
  }
  setupToggle('flashCaptureToggle', 'flashOnCapture');
  setupToggle('soundToggle', 'soundOn');
  setupToggle('autoMirrorToggle', 'autoMirror');

  // ========== Keyboard Shortcuts ==========
  document.addEventListener('keydown', (e) => {
    if (e.code === 'Space') {
      e.preventDefault();
      captureBtn.click();
    } else if (e.code === 'KeyG') {
      document.getElementById('gridBtn').click();
    } else if (e.code === 'KeyS') {
      switchBtn.click();
    } else if (e.code === 'KeyM') {
      document.getElementById('mirrorBtn').click();
    } else if (e.code === 'Escape') {
      galleryPanel.classList.remove('open');
      previewModal.classList.remove('open');
      settingsPanel.classList.remove('open');
    }
  });

  // ========== Background animation for atmosphere ==========
  const glows = document.querySelectorAll('.bg-glow');
  let glowAngle = 0;
  function animateGlow() {
    glowAngle += 0.005;
    glows[0].style.transform = `translate(${Math.sin(glowAngle) * 30}px, ${Math.cos(glowAngle) * 20}px)`;
    glows[1].style.transform = `translate(${Math.cos(glowAngle) * 25}px, ${Math.sin(glowAngle) * 35}px)`;
    requestAnimationFrame(animateGlow);
  }
  animateGlow();

  // ========== Init ==========
  startCamera();
</script>
</body>
</html>
