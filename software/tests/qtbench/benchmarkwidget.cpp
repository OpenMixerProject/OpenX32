#include "benchmarkwidget.h"
#include <QPainter>
#include <QTimerEvent>
#include <QResizeEvent>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QRandomGenerator>
#include <cmath>

namespace {
double randomBetween(double min, double max)
{
    if (max <= min) {
        return min;
    }

    return min + QRandomGenerator::global()->generateDouble() * (max - min);
}
}

BenchmarkArea::BenchmarkArea(QWidget *parent)
    : QWidget(parent)
    , m_currentTest(TestVUMeters)
    , m_antiAliasing(false)
    , m_uncappedFps(false)
    , m_elementCount(16)
    , m_frameCount(0)
    , m_fps(0.0)
    , m_avgFrameTimeMs(0.0)
    , m_lastTickTime(0)
    , m_animationTime(0.0)
{
    // Performance optimization: tell Qt we will paint the whole widget
    setAttribute(Qt::WA_OpaquePaintEvent);
    
    m_fpsTimer.start();
    m_frameTimer.start();
    m_lastTickTime = m_frameTimer.elapsed();
    
    // Start with capped FPS (16ms = ~60 FPS)
    m_timerId = startTimer(16);
    
    initParticles();
}

void BenchmarkArea::setTestType(TestType type) {
    if (m_currentTest != type) {
        m_currentTest = type;
        if (m_currentTest == TestParticles) {
            initParticles();
        }
        update();
    }
}

void BenchmarkArea::setAntiAliasing(bool enabled) {
    if (m_antiAliasing != enabled) {
        m_antiAliasing = enabled;
        update();
    }
}

void BenchmarkArea::setUncappedFps(bool uncapped) {
    if (m_uncappedFps != uncapped) {
        m_uncappedFps = uncapped;
        killTimer(m_timerId);
        if (m_uncappedFps) {
            m_timerId = startTimer(0); // Run as fast as possible
        } else {
            m_timerId = startTimer(16); // ~60 FPS
        }
    }
}

void BenchmarkArea::setElementCount(int count) {
    if (count < 1) count = 1;
    if (count > 200) count = 200; // Sensible limit
    if (m_elementCount != count) {
        m_elementCount = count;
        if (m_currentTest == TestParticles) {
            initParticles();
        }
        update();
    }
}

void BenchmarkArea::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_currentTest == TestParticles) {
        initParticles();
    }
}

void BenchmarkArea::initParticles() {
    m_particles.clear();
    int w = qMax(100, width());
    int h = qMax(100, height());
    
    for (int i = 0; i < m_elementCount; ++i) {
        Particle p;
        p.radius = randomBetween(10.0, 35.0);
        p.position = QPointF(randomBetween(p.radius, w - p.radius),
                             randomBetween(p.radius, h - p.radius));
        // Velocity in pixels per second
        p.velocity = QPointF(randomBetween(-200.0, 200.0),
                             randomBetween(-200.0, 200.0));
        if (p.velocity.x() == 0) p.velocity.setX(50);
        if (p.velocity.y() == 0) p.velocity.setY(50);
        
        // Vibrant neon-like colors
        int colorType = QRandomGenerator::global()->bounded(4);
        if (colorType == 0) p.color = QColor(0, 255, 204, 160);      // Cyan
        else if (colorType == 1) p.color = QColor(255, 0, 128, 160); // Pink/Magenta
        else if (colorType == 2) p.color = QColor(255, 153, 0, 160); // Orange
        else p.color = QColor(153, 51, 255, 160);                    // Purple
        
        m_particles.append(p);
    }
}

void BenchmarkArea::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_timerId) {
        updatePhysics();
        update(); // Request repaint
    }
}

void BenchmarkArea::updatePhysics() {
    qint64 currentTime = m_frameTimer.elapsed();
    double dt = (currentTime - m_lastTickTime) / 1000.0; // delta time in seconds
    m_lastTickTime = currentTime;
    
    // Cap dt to prevent huge jumps if debugger is paused or system hangs
    if (dt > 0.1) dt = 0.1;
    
    m_animationTime += dt;
    
    if (m_currentTest == TestParticles) {
        int w = width();
        int h = height();
        for (int i = 0; i < m_particles.size(); ++i) {
            Particle &p = m_particles[i];
            p.position += p.velocity * dt;
            
            // Boundary checks
            if (p.position.x() - p.radius < 0) {
                p.position.setX(p.radius);
                p.velocity.setX(-p.velocity.x());
            } else if (p.position.x() + p.radius > w) {
                p.position.setX(w - p.radius);
                p.velocity.setX(-p.velocity.x());
            }
            
            if (p.position.y() - p.radius < 0) {
                p.position.setY(p.radius);
                p.velocity.setY(-p.velocity.y());
            } else if (p.position.y() + p.radius > h) {
                p.position.setY(h - p.radius);
                p.velocity.setY(-p.velocity.y());
            }
        }
    }
}

void BenchmarkArea::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    // Track frame rate
    m_frameCount++;
    qint64 elapsed = m_fpsTimer.elapsed();
    if (elapsed >= 1000) {
        m_fps = (m_frameCount * 1000.0) / elapsed;
        m_avgFrameTimeMs = (double)elapsed / m_frameCount;
        m_frameCount = 0;
        m_fpsTimer.restart();
    }
    
    QPainter painter(this);
    
    // Enable rendering options
    if (m_antiAliasing) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
    }
    
    // Fill background
    painter.fillRect(rect(), QColor("#121214"));
    
    // Draw active test
    switch (m_currentTest) {
        case TestVUMeters:
            drawVUMeters(painter);
            break;
        case TestFaders:
            drawFaders(painter);
            break;
        case TestParticles:
            drawParticles(painter);
            break;
        case TestText:
            drawTextBenchmark(painter);
            break;
        default:
            break;
    }
    
}

void BenchmarkArea::drawVUMeters(QPainter &painter) {
    int w = width();
    int h = height();
    int count = m_elementCount;
    
    double colWidth = (double)w / count;
    double padding = qMax(2.0, colWidth * 0.05);
    double meterWidth = (colWidth - padding * 2) / 2.0; // Stereo (L and R)
    
    int numSegments = 25;
    double segmentPadding = 2.0;
    double segmentHeight = (h - 60.0 - (numSegments * segmentPadding)) / numSegments;
    
    for (int col = 0; col < count; ++col) {
        double colX = col * colWidth;
        
        // Draw channel background
        painter.fillRect(QRectF(colX + padding, 10, colWidth - padding * 2, h - 50), QColor("#1A1A1E"));
        
        // Compute meter levels using sine waves (to simulate shifting volume)
        double phaseL = col * 0.5;
        double phaseR = col * 0.5 + 0.25;
        double levelL = (std::sin(m_animationTime * 3.5 + phaseL) + 1.0) / 2.0; // 0 to 1
        double levelR = (std::sin(m_animationTime * 4.2 + phaseR) + 1.0) / 2.0;
        
        // Occasional transient spike
        if (std::fmod(m_animationTime + col * 0.7, 4.0) > 3.7) {
            levelL = qMin(1.0, levelL * 1.3);
            levelR = qMin(1.0, levelR * 1.3);
        }
        
        int activeSegsL = levelL * numSegments;
        int activeSegsR = levelR * numSegments;
        
        // Draw L and R meter bars
        for (int ch = 0; ch < 2; ++ch) {
            double meterX = colX + padding + (ch * meterWidth) + (ch * 0.5);
            int activeSegs = (ch == 0) ? activeSegsL : activeSegsR;
            
            for (int seg = 0; seg < numSegments; ++seg) {
                double segY = h - 50 - (seg * (segmentHeight + segmentPadding)) - segmentHeight;
                
                // Color mapping: 0-15 Green, 16-21 Yellow, 22-24 Red
                QColor segColor;
                bool isActive = (seg < activeSegs);
                
                if (seg < 16) {
                    segColor = isActive ? QColor(0, 220, 100) : QColor(0, 60, 25);
                } else if (seg < 22) {
                    segColor = isActive ? QColor(240, 200, 0) : QColor(60, 50, 0);
                } else {
                    segColor = isActive ? QColor(255, 50, 50) : QColor(70, 15, 15);
                }
                
                painter.fillRect(QRectF(meterX, segY, meterWidth - 1, segmentHeight), segColor);
            }
        }
        
        // Channel text label
        painter.setPen(QColor("#8A8A93"));
        QFont font = painter.font();
        font.setPixelSize(qMax(8.0, qMin(12.0, colWidth * 0.25)));
        painter.setFont(font);
        
        QString label = QString("CH%1").arg(col + 1, 2, 10, QChar('0'));
        painter.drawText(QRectF(colX + padding, h - 35, colWidth - padding * 2, 20), Qt::AlignCenter, label);
    }
}

void BenchmarkArea::drawFaders(QPainter &painter) {
    int w = width();
    int h = height();
    int count = m_elementCount;
    
    double colWidth = (double)w / count;
    double padding = qMax(4.0, colWidth * 0.08);
    
    double trackTop = 80.0;
    double trackBottom = h - 60.0;
    double trackHeight = trackBottom - trackTop;
    
    for (int col = 0; col < count; ++col) {
        double colX = col * colWidth;
        double centerX = colX + colWidth / 2.0;
        
        // Draw channel card background
        painter.fillRect(QRectF(colX + padding, 10, colWidth - padding * 2, h - 30), QColor("#161619"));
        
        // 1. Draw Pan Knob (above the fader)
        double knobY = 35.0;
        double knobRadius = qMin(colWidth * 0.2, 15.0);
        if (knobRadius > 4.0) {
            painter.setPen(QPen(QColor("#52525B"), 2));
            painter.setBrush(QColor("#27272A"));
            painter.drawEllipse(QPointF(centerX, knobY), knobRadius, knobRadius);
            
            // Knob pointer line
            double knobAngle = std::sin(m_animationTime * 1.5 + col * 0.4) * 1.2; // in radians
            QPointF pointerEnd(centerX + knobRadius * std::sin(knobAngle), knobY - knobRadius * std::cos(knobAngle));
            painter.setPen(QPen(QColor("#00FFCC"), 2));
            painter.drawLine(QPointF(centerX, knobY), pointerEnd);
        }
        
        // 2. Draw Fader Track Line
        painter.setPen(QPen(QColor("#27272A"), 3));
        painter.drawLine(QPointF(centerX, trackTop), QPointF(centerX, trackBottom));
        
        // Draw tick marks
        painter.setPen(QPen(QColor("#3F3F46"), 1));
        for (double val = 0.0; val <= 1.0; val += 0.2) {
            double tickY = trackTop + val * trackHeight;
            double tickLen = qMin(colWidth * 0.15, 8.0);
            painter.drawLine(QPointF(centerX - tickLen, tickY), QPointF(centerX + tickLen, tickY));
        }
        
        // 3. Compute animated fader position
        double faderPosNorm = (std::sin(m_animationTime * 0.8 + col * 0.3) + 1.0) / 2.0; // 0.0 to 1.0
        double faderY = trackTop + faderPosNorm * trackHeight;
        
        // Draw Fader Cap
        double capW = qMin(colWidth * 0.7, 40.0);
        double capH = qMin(colWidth * 0.4, 24.0);
        
        if (capW > 4.0 && capH > 4.0) {
            QRectF capRect(centerX - capW / 2.0, faderY - capH / 2.0, capW, capH);
            
            // Fader Cap Metallic Gradient
            QLinearGradient gradient(capRect.topLeft(), capRect.bottomLeft());
            gradient.setColorAt(0.0, QColor("#A1A1AA"));
            gradient.setColorAt(0.4, QColor("#52525B"));
            gradient.setColorAt(0.6, QColor("#3F3F46"));
            gradient.setColorAt(1.0, QColor("#18181B"));
            
            painter.setPen(QPen(QColor("#71717A"), 1));
            painter.setBrush(gradient);
            painter.drawRoundedRect(capRect, 3, 3);
            
            // Red center line
            painter.setPen(QPen(QColor("#FF3366"), 2));
            painter.drawLine(QPointF(centerX - capW / 2.0 + 2, faderY), QPointF(centerX + capW / 2.0 - 2, faderY));
        }
        
        // Label fader
        painter.setPen(QColor("#71717A"));
        QFont font = painter.font();
        font.setPixelSize(qMax(6.0, qMin(10.0, colWidth * 0.2)));
        painter.setFont(font);
        
        QString valText = QString("%1dB").arg((1.0 - faderPosNorm) * 20.0 - 10.0, 0, 'f', 1);
        painter.drawText(QRectF(colX + padding, h - 25, colWidth - padding * 2, 15), Qt::AlignCenter, valText);
    }
}

void BenchmarkArea::drawParticles(QPainter &painter) {
    // Fill transparent elements to test blending
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < m_particles.size(); ++i) {
        const Particle &p = m_particles[i];
        painter.setBrush(p.color);
        painter.drawEllipse(p.position, p.radius, p.radius);
        
        // Add a glossy highlighted border inside
        if (m_antiAliasing) {
            painter.setPen(QPen(QColor(255, 255, 255, 80), 1.5));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(p.position, p.radius - 2.0, p.radius - 2.0);
            painter.setPen(Qt::NoPen);
        }
    }
}

void BenchmarkArea::drawTextBenchmark(QPainter &painter) {
    int w = width();
    int h = height();
    
    // Draw scrolling background text lines
    painter.setPen(QColor(40, 40, 50, 90));
    QFont font = painter.font();
    font.setFamily("Courier New");
    font.setPixelSize(14);
    font.setBold(true);
    painter.setFont(font);
    
    int lineSpacing = 18;
    int scrollOffset = std::fmod(m_animationTime * 40.0, lineSpacing);
    
    for (int y = -lineSpacing; y < h; y += lineSpacing) {
        QString textLine = "OpenX32 Benchmark - i.MX253 ARM9 @ 400MHz - Qt Performance Analysis Tool - ";
        textLine = textLine.repeated(3);
        painter.drawText(0, y + scrollOffset, textLine);
    }
    
    // Draw foreground stats panel
    QRectF panelRect(w * 0.1, h * 0.2, w * 0.8, h * 0.6);
    painter.setPen(QPen(QColor("#00FFCC"), 1.5));
    painter.setBrush(QColor(15, 15, 20, 225));
    painter.drawRoundedRect(panelRect, 8, 8);
    
    // Title inside panel
    painter.setPen(QColor("#FFFFFF"));
    font.setFamily("Helvetica");
    font.setPixelSize(22);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(panelRect, Qt::AlignTop | Qt::AlignHCenter, "\nQT GRAPHICS BENCHMARK");
    
    // Floating rotating/scaling element in the center of the panel
    double scale = 1.0 + 0.15 * std::sin(m_animationTime * 2.5);
    double rotation = m_animationTime * 45.0; // degrees
    
    painter.save();
    painter.translate(w / 2.0, h / 2.0 + 20);
    painter.rotate(rotation);
    painter.scale(scale, scale);
    
    font.setPixelSize(18);
    font.setBold(false);
    painter.setFont(font);
    painter.setPen(QColor("#FF3366"));
    painter.drawText(QRect(-150, -15, 300, 30), Qt::AlignCenter, "BEHRINGER X32");
    
    painter.restore();
    
    // Renders multiple blocks of small text to test layout/rendering performance
    painter.setPen(QColor("#A1A1AA"));
    font.setPixelSize(11);
    font.setBold(false);
    painter.setFont(font);
    
    QString descText = "Testing Qt Font Engine, Glyphs cache, and Layout text formatting.\n"
                       "This console runs Linux kernel 6.18 LTS and builds natively on musl-libc.\n"
                       "Qt leverages hardware framebuffers directly using linuxfb platform integration.\n"
                       "Adjust anti-aliasing and element count to compare CPU rasterization load.";
    
    painter.drawText(panelRect.adjusted(20, 20, -20, -20), Qt::AlignBottom | Qt::AlignLeft, descText);
}

// ----------------------------------------------------------------------------
// Main BenchmarkWidget Implementation
// ----------------------------------------------------------------------------

BenchmarkWidget::BenchmarkWidget(QWidget *parent)
    : QWidget(parent)
{
    // Sleek styling for the window itself
    setStyleSheet(
        "QWidget { background-color: #0F0F11; color: #E4E4E7; font-family: 'Outfit', 'Inter', sans-serif; }"
        "QPushButton { background-color: #27272A; border: 1px solid #3F3F46; border-radius: 4px; padding: 6px 12px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #3F3F46; border-color: #71717A; }"
        "QPushButton:pressed { background-color: #18181B; }"
        "QLabel { font-size: 13px; font-weight: 500; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);
    
    // Top Bar Layout
    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->setSpacing(8);
    
    m_btnCycle = new QPushButton("Next Test", this);
    connect(m_btnCycle, &QPushButton::clicked, this, &BenchmarkWidget::onCycleTest);
    topBar->addWidget(m_btnCycle);
    
    m_btnAA = new QPushButton("AA: OFF", this);
    connect(m_btnAA, &QPushButton::clicked, this, &BenchmarkWidget::onToggleAA);
    topBar->addWidget(m_btnAA);
    
    m_btnFpsCap = new QPushButton("VSync: ON", this);
    connect(m_btnFpsCap, &QPushButton::clicked, this, &BenchmarkWidget::onToggleFpsCap);
    topBar->addWidget(m_btnFpsCap);
    
    m_btnDec = new QPushButton("- Elements", this);
    connect(m_btnDec, &QPushButton::clicked, this, &BenchmarkWidget::onDecreaseElements);
    topBar->addWidget(m_btnDec);
    
    m_btnInc = new QPushButton("+ Elements", this);
    connect(m_btnInc, &QPushButton::clicked, this, &BenchmarkWidget::onIncreaseElements);
    topBar->addWidget(m_btnInc);
    
    topBar->addStretch();
    
    mainLayout->addLayout(topBar);
    
    // Center rendering area
    m_benchmarkArea = new BenchmarkArea(this);
    mainLayout->addWidget(m_benchmarkArea, 1); // Expand to fill
    
    // Bottom status bar
    QHBoxLayout *bottomBar = new QHBoxLayout();
    m_lblStats = new QLabel("FPS: --  |  Frame Time: -- ms  |  Elements: 16", this);
    m_lblStats->setStyleSheet("color: #00FFCC; font-weight: bold; font-family: monospace;");
    bottomBar->addWidget(m_lblStats);
    
    mainLayout->addLayout(bottomBar);
    
    // Timer to update stats labels every 500ms
    m_statsTimer = new QTimer(this);
    connect(m_statsTimer, &QTimer::timeout, this, &BenchmarkWidget::updateStats);
    m_statsTimer->start(500);
}

void BenchmarkWidget::onCycleTest() {
    int next = (int)m_benchmarkArea->testType() + 1;
    if (next >= BenchmarkArea::TestCount) next = 0;
    m_benchmarkArea->setTestType((BenchmarkArea::TestType)next);
    
    // Dynamically adjust default elements based on scene type
    if (m_benchmarkArea->testType() == BenchmarkArea::TestParticles) {
        m_benchmarkArea->setElementCount(40);
    } else if (m_benchmarkArea->testType() == BenchmarkArea::TestText) {
        m_benchmarkArea->setElementCount(1);
    } else {
        m_benchmarkArea->setElementCount(16);
    }
}

void BenchmarkWidget::onToggleAA() {
    bool aa = !m_benchmarkArea->antiAliasing();
    m_benchmarkArea->setAntiAliasing(aa);
    m_btnAA->setText(aa ? "AA: ON" : "AA: OFF");
    m_btnAA->setStyleSheet(aa ? "background-color: #0d5c4b; border-color: #00FFCC; color: #FFFFFF;" : "");
}

void BenchmarkWidget::onToggleFpsCap() {
    bool uncapped = !m_benchmarkArea->uncappedFps();
    m_benchmarkArea->setUncappedFps(uncapped);
    m_btnFpsCap->setText(uncapped ? "VSync: OFF" : "VSync: ON");
    m_btnFpsCap->setStyleSheet(uncapped ? "background-color: #7c2d12; border-color: #ea580c; color: #FFFFFF;" : "");
}

void BenchmarkWidget::onIncreaseElements() {
    int count = m_benchmarkArea->elementCount();
    if (m_benchmarkArea->testType() == BenchmarkArea::TestParticles) {
        m_benchmarkArea->setElementCount(count + 10);
    } else {
        m_benchmarkArea->setElementCount(count + 4);
    }
}

void BenchmarkWidget::onDecreaseElements() {
    int count = m_benchmarkArea->elementCount();
    if (m_benchmarkArea->testType() == BenchmarkArea::TestParticles) {
        m_benchmarkArea->setElementCount(count - 10);
    } else {
        m_benchmarkArea->setElementCount(count - 4);
    }
}

void BenchmarkWidget::updateStats() {
    QString testName;
    switch (m_benchmarkArea->testType()) {
        case BenchmarkArea::TestVUMeters:  testName = "VU Meters"; break;
        case BenchmarkArea::TestFaders:    testName = "Mixer Faders"; break;
        case BenchmarkArea::TestParticles: testName = "Particles"; break;
        case BenchmarkArea::TestText:      testName = "Text Rendering"; break;
        default:                           testName = "Unknown"; break;
    }
    
    m_lblStats->setText(QString("Scene: %1  |  FPS: %2  |  Time: %3 ms  |  Elements: %4")
                        .arg(testName)
                        .arg(m_benchmarkArea->currentFps(), 0, 'f', 1)
                        .arg(m_benchmarkArea->averageFrameTimeMs(), 0, 'f', 2)
                        .arg(m_benchmarkArea->elementCount()));
}
