#ifndef BENCHMARKWIDGET_H
#define BENCHMARKWIDGET_H

#include <QWidget>
#include <QElapsedTimer>
#include <QVector>
#include <QPointF>
#include <QColor>

class QPainter;
class QTimer;

// Helper structure for particle benchmark
struct Particle {
    QPointF position;
    QPointF velocity;
    double radius;
    QColor color;
};

// The area where rendering benchmarks actually run
class BenchmarkArea : public QWidget {
    Q_OBJECT
public:
    enum TestType {
        TestVUMeters,
        TestFaders,
        TestParticles,
        TestText,
        TestCount // Helper for cycling
    };

    explicit BenchmarkArea(QWidget *parent = nullptr);

    void setTestType(TestType type);
    TestType testType() const { return m_currentTest; }
    
    void setAntiAliasing(bool enabled);
    bool antiAliasing() const { return m_antiAliasing; }

    void setElementCount(int count);
    int elementCount() const { return m_elementCount; }

    double currentFps() const { return m_fps; }
    double averageFrameTimeMs() const { return m_avgFrameTimeMs; }

    void setUncappedFps(bool uncapped);
    bool uncappedFps() const { return m_uncappedFps; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawVUMeters(QPainter &painter);
    void drawFaders(QPainter &painter);
    void drawParticles(QPainter &painter);
    void drawTextBenchmark(QPainter &painter);

    void updatePhysics();
    void initParticles();

    TestType m_currentTest;
    bool m_antiAliasing;
    bool m_uncappedFps;
    int m_elementCount;
    int m_timerId;

    // Performance measurements
    QElapsedTimer m_fpsTimer;
    QElapsedTimer m_frameTimer;
    int m_frameCount;
    double m_fps;
    double m_avgFrameTimeMs;
    qint64 m_lastTickTime;

    // Animation states
    double m_animationTime;
    QVector<Particle> m_particles;
};

// Main container window
class QPushButton;
class QLabel;

class BenchmarkWidget : public QWidget {
    Q_OBJECT
public:
    explicit BenchmarkWidget(QWidget *parent = nullptr);

private slots:
    void onCycleTest();
    void onToggleAA();
    void onToggleFpsCap();
    void onIncreaseElements();
    void onDecreaseElements();
    void updateStats();

private:
    BenchmarkArea *m_benchmarkArea;
    QPushButton *m_btnCycle;
    QPushButton *m_btnAA;
    QPushButton *m_btnFpsCap;
    QPushButton *m_btnInc;
    QPushButton *m_btnDec;
    QLabel *m_lblStats;
    QTimer *m_statsTimer;
};

#endif // BENCHMARKWIDGET_H
