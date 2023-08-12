- Update README.md
- Relax max sizes for real word scenarios
- Instead of hardcoded 8 for bits in byte we can use define that is defined somewhere.
- Deleting/Modifying global elements
    - E.g., when we merge text nodes during parsing.

- Is it correct to only *not* search in the global text for new text or should we also search there? Or other globals where searching is discouraged?

- deal with unused functions. Maybe only allow in debug builds? not sure.
