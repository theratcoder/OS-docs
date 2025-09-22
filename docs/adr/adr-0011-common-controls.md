# ADR-0011: Common Controls Strategy

## Status
Proposed

---

## Context
RatAPI provides the foundational system API for RatOS, including graphics, input, and window management. However, exposing only raw drawing surfaces and event handling would leave developers with the burden of implementing common UI elements (buttons, text fields, menus, etc.) from scratch.
Operating systems such as Windows (via the Common Controls library) and toolkits like GTK/Qt demonstrate the importance of providing a standard set of prebuilt widgets to accelerate development, ensure consistency, and promote a cohesive user experience.

The challenge is balancing developer productivity, system cohesiveness, and API complexity/maintenance burden.

---

## Options Considered

- A: Minimal Core Only → Buttons, text fields, menus, scrollbars. Lightweight, but limited.

- B: Rich Control Suite → Wide range of widgets. Developer-friendly but heavy to implement/maintain.

- C: Fully Themed/Skinnable → Flexible, but adds complexity up front.

- D: Hybrid Model (Core + Extended) → Minimal mandatory controls plus an optional extended library.

- E: No Common Controls → Leave it entirely to developers (fragmentation risk).

---

## Decision
RatOS will adopt Option D (Hybrid Model).

Core Controls (always available in RatAPI):

- Button

- Label (static text)

- Text box (single/multi-line edit control)

- Menu (basic menus and context menus)

- Scrollbar

Extended Controls (optional library, distributed with RatOS):

- List box / List view

- Tree view

- Combo box

- Tabs

- Progress bar

- Toolbar / Status bar

The extended library will be loadable as a shared library (libratcontrols.so) so lightweight apps don’t pay for features they don’t use.

---

## Rationale

Developers get a usable set of widgets out of the box (avoiding the "blank canvas" trap).

The extended library allows richer UIs without bloating the core RatAPI.

This mirrors the historical split between Win32 USER32 (core) and COMCTL32 (extended controls).

A future theming/skinning API can evolve on top of this foundation without breaking apps.

---

## Consequences

RatOS must implement and maintain at least the core controls.

Extended controls can evolve more rapidly, with versioning handled via dynamic libraries.

Third-party widget toolkits can still be built on top of RatAPI without conflict.

Developers retain freedom: minimal apps stay lean, desktop apps can use richer controls.