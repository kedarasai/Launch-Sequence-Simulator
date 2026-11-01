#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define STAGE1_ACCEL 15.0
#define STAGE2_ACCEL 25.0
#define STAGE1_BURN_TIME 120
#define ORBIT_SPEED 7800.0
#define PITCH_TIME 360.0
#define MAX_BURN_TIME 600
#define TICK_MS 50

typedef enum {
    STATE_IDLE,
    STATE_CHECKS,
    STATE_COUNTDOWN,
    STATE_IGNITION,
    STATE_LIFTOFF,
    STATE_SEPARATION,
    STATE_ORBIT,
    STATE_SUCCESS,
    STATE_ABORT
} State;

typedef struct {
    int fault;
    int fast;
    int airborne;
    int time;
    double speed;
    double altitude;
    const char *reason;
} Mission;

void sleep_ms(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

void delay(const Mission *m, int ms)
{
    if (!m->fast)
        sleep_ms(ms);
}

void clear_input(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int get_int(const char *msg, int min, int max)
{
    int x, ok;

    while (1) {
        printf("%s", msg);
        ok = scanf("%d", &x);
        if (ok == 1 && x >= min && x <= max)
            return x;
        if (ok == EOF) {
            printf("\nInput ended.\n");
            exit(1);
        }
        clear_input();
        printf("Enter a whole number from %d to %d.\n", min, max);
    }
}

void print_header(void)
{
    printf("\n %6s %14s %16s %8s\n", "T+ (s)", "Speed (m/s)", "Altitude (km)", "Stage");
}

void print_telemetry(const Mission *m, int stage)
{
    printf(" %6d %14.0f %16.1f %8d\n", m->time, m->speed, m->altitude, stage);
}

void advance_one_second(Mission *m, double accel)
{
    double old_speed = m->speed;
    double climb_share;

    m->speed += accel;
    m->time++;

    climb_share = 1.0 - m->time / PITCH_TIME;
    if (climb_share < 0.0)
        climb_share = 0.0;

    m->altitude += 0.5 * (old_speed + m->speed) * climb_share / 1000.0;
}

State run_checks(Mission *m)
{
    const char *names[5] = {
        "Fuel tank pressure",
        "Guidance system",
        "Communication link",
        "Weather conditions",
        "Range clearance"
    };
    int i;

    printf("\n[PRE-LAUNCH CHECKS]\n");
    for (i = 0; i < 5; i++) {
        printf("  %-20s ... ", names[i]);
        fflush(stdout);
        delay(m, 700);
        if (i == 0 && m->fault == 1) {
            printf("FAIL\n");
            m->reason = "Fuel tank pressure below the safe limit.";
            return STATE_ABORT;
        }
        printf("OK\n");
    }
    return STATE_COUNTDOWN;
}

State run_countdown(const Mission *m)
{
    int t;

    printf("\n[COUNTDOWN]\n");
    for (t = 10; t >= 1; t--) {
        printf("  T-%d\n", t);
        fflush(stdout);
        delay(m, 1000);
    }
    return STATE_IGNITION;
}

State run_ignition(Mission *m)
{
    printf("\n[IGNITION]\n");
    printf("  Engine start command sent\n");
    fflush(stdout);
    delay(m, 1000);

    if (m->fault == 2) {
        printf("  Engine thrust not reached\n");
        m->reason = "Engine ignition failure. Engines shut down on the pad.";
        return STATE_ABORT;
    }

    printf("  Thrust nominal. Hold-down released.\n");
    return STATE_LIFTOFF;
}

State run_liftoff(Mission *m)
{
    printf("\n[LIFTOFF] Stage 1 burning\n");
    m->airborne = 1;
    print_header();
    print_telemetry(m, 1);

    while (m->time < STAGE1_BURN_TIME) {
        advance_one_second(m, STAGE1_ACCEL);
        if (m->time % 10 == 0)
            print_telemetry(m, 1);
        fflush(stdout);
        delay(m, TICK_MS);
    }
    return STATE_SEPARATION;
}

State run_separation(Mission *m)
{
    printf("\n[STAGE SEPARATION]\n");
    printf("  Stage 1 burnout\n");
    fflush(stdout);
    delay(m, 1000);

    if (m->fault == 3) {
        printf("  Separation signal sent, stage 1 did not detach\n");
        m->reason = "Stage separation failure. Vehicle lost in flight.";
        return STATE_ABORT;
    }

    printf("  Stage 1 separated\n");
    printf("  Stage 2 ignition confirmed\n");
    delay(m, 1000);
    return STATE_ORBIT;
}

State run_orbit_insertion(Mission *m)
{
    printf("\n[ORBIT INSERTION] Stage 2 burning\n");
    print_header();

    while (m->speed < ORBIT_SPEED && m->time < MAX_BURN_TIME) {
        advance_one_second(m, STAGE2_ACCEL);
        if (m->time % 10 == 0)
            print_telemetry(m, 2);
        fflush(stdout);
        delay(m, TICK_MS);
    }

    if (m->speed >= ORBIT_SPEED)
        return STATE_SUCCESS;

    m->reason = "Orbital speed not reached before the burn time limit.";
    return STATE_ABORT;
}

void run_mission(int fault, int fast)
{
    Mission m;
    State state = STATE_IDLE;

    m.fault = fault;
    m.fast = fast;
    m.airborne = 0;
    m.time = 0;
    m.speed = 0.0;
    m.altitude = 0.0;
    m.reason = "";

    while (state != STATE_SUCCESS && state != STATE_ABORT) {
        switch (state) {
        case STATE_IDLE:
            printf("\n[IDLE] Vehicle on the pad. Starting launch sequence.\n");
            delay(&m, 1000);
            state = STATE_CHECKS;
            break;
        case STATE_CHECKS:
            state = run_checks(&m);
            break;
        case STATE_COUNTDOWN:
            state = run_countdown(&m);
            break;
        case STATE_IGNITION:
            state = run_ignition(&m);
            break;
        case STATE_LIFTOFF:
            state = run_liftoff(&m);
            break;
        case STATE_SEPARATION:
            state = run_separation(&m);
            break;
        case STATE_ORBIT:
            state = run_orbit_insertion(&m);
            break;
        default:
            state = STATE_ABORT;
            break;
        }
    }

    if (state == STATE_SUCCESS) {
        printf("\n[MISSION SUCCESS]\n");
        printf("  Orbit achieved at %.1f km altitude\n", m.altitude);
        printf("  Orbital speed    : %.0f m/s\n", m.speed);
        printf("  Total burn time  : %d s\n", m.time);
        printf("  Satellite deployed\n");
    } else {
        printf("\n[MISSION ABORTED]\n");
        printf("  Reason: %s\n", m.reason);
        if (m.airborne)
            printf("  Status: vehicle lost at T+%d s\n", m.time);
        else
            printf("  Status: launch scrubbed, vehicle safe on the pad\n");
    }
}

int main(void)
{
    int choice, fault, mode;

    printf("Launch Sequencer Simulator\n");
    printf("(simplified model for learning, not real flight data)\n");

    while (1) {
        printf("\n1. Normal launch\n");
        printf("2. Launch with a simulated fault\n");
        printf("3. Exit\n");
        choice = get_int("Choice: ", 1, 3);

        if (choice == 3) {
            printf("Goodbye.\n");
            break;
        }

        fault = 0;
        if (choice == 2) {
            printf("\n1. Low fuel pressure\n");
            printf("2. Engine ignition failure\n");
            printf("3. Stage separation failure\n");
            fault = get_int("Which fault: ", 1, 3);
        }

        printf("\n1. Real-time delays (about 35 seconds)\n");
        printf("2. Fast mode (no waiting)\n");
        mode = get_int("Speed: ", 1, 2);

        run_mission(fault, mode == 2);
    }
    return 0;
}