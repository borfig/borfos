/*
 *  borfos - a minimal OS for x86
 *  Copyright (C) 2013 Boris Figovsky.
 *
 *  borfos is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  borfos is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with borfos.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cpuid.h>
#include <ctors.h>
#include <pvclock-abi.h>
#include <asm.h>
#include <mem.h>
#include <kprintf.h>
#include <lapic.h>

#define KVM_CPUID_SIGNATURE 0x40000000

static pvclock_wall_clock_t wall_clock __attribute__((aligned(4)));
static uint64_t wall_clock_nsecs;
static pvclock_vcpu_time_info_t system_time __attribute__((aligned(4)));

uint64_t tsc_to_nanoseconds(uint64_t tsc)
{
    uint64_t r = tsc;
    if (system_time.tsc_shift >= 0)
        r <<= system_time.tsc_shift;
    else
        r >>= -system_time.tsc_shift;
    return (r * system_time.tsc_to_system_mul) >> 32;
}

uint64_t now_in_nanoseconds(void)
{
    uint64_t b = wall_clock_nsecs + system_time.system_time;
    return b + tsc_to_nanoseconds(rdtsc()-system_time.tsc_timestamp);
}

uint64_t nanoseconds_to_tsc(uint64_t ns)
{
    uint64_t r = (ns << 32) / system_time.tsc_to_system_mul;
    if (system_time.tsc_shift >= 0)
        r >>= system_time.tsc_shift;
    else
        r <<= -system_time.tsc_shift;
    return r;
}

static unsigned __attribute__((aligned(4))) kvm_apic_eoi;

static inline int test_and_clear_bit(int nr, volatile uint32_t *addr)
{
    int oldbit;
    __asm__ __volatile__("btr %2,%1\n\t"
                         "sbb %0,%0"
                         : "=r" (oldbit), "+m"(addr)
                         : "Ir" (nr));
    return oldbit;
}

static void kvm_lapic_eoi(void)
{
    if (test_and_clear_bit(0, &kvm_apic_eoi)) {
        return;
    }
    _lapic_eoi();
}


#define KVM_FEATURE_CLOCKSOURCE		0
#define KVM_FEATURE_CLOCKSOURCE2        3
#define KVM_FEATURE_PV_EOI		6

#define MSR_KVM_WALL_CLOCK  0x11
#define MSR_KVM_SYSTEM_TIME 0x12
#define MSR_KVM_WALL_CLOCK_NEW  0x4b564d00
#define MSR_KVM_SYSTEM_TIME_NEW 0x4b564d01
#define MSR_KVM_PV_EOI_EN       0x4b564d04

CONSTRUCTOR(kvm)
{
    uint32_t eax, ebx, ecx, edx;

    CPUID(KVM_CPUID_SIGNATURE, eax, ebx, ecx, edx);
    if (0x4b4d564b != ebx ||
        0x564b4d56 != ecx ||
        0x0000004d != edx) {
        PANIC("Your CPU is not supported: it is not virtual KVM CPU");
    }

    CPUIDA(KVM_CPUID_SIGNATURE+1, eax);

    uint32_t wall_clock_msr, system_time_msr;
    if (eax & (1 << KVM_FEATURE_CLOCKSOURCE2)) {
        wall_clock_msr = MSR_KVM_WALL_CLOCK_NEW;
        system_time_msr = MSR_KVM_SYSTEM_TIME_NEW;
    }
    else if (eax & (1 << KVM_FEATURE_CLOCKSOURCE)) {
        wall_clock_msr = MSR_KVM_WALL_CLOCK;
        system_time_msr = MSR_KVM_SYSTEM_TIME;
    }
    else
        PANIC("Your virtual KVM CPU is not supported: it does not have a KVM clock");

    wrmsr32(wall_clock_msr, PA((uint32_t)&wall_clock));
    wall_clock_nsecs = wall_clock.sec * 1000000000ull + wall_clock.nsec;
    wrmsr32(system_time_msr, PA((uint32_t)&system_time)+1);

    if (eax & (1 << KVM_FEATURE_PV_EOI)) {
        wrmsr32(MSR_KVM_PV_EOI_EN, PA((uint32_t)&kvm_apic_eoi) | 1);
        lapic_eoi = kvm_lapic_eoi;

    }
}
