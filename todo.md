- fix printing of node comparisons on failure.

- Deleting/Modifying global elements
    - E.g., when we merge text nodes during parsing.

- Is it correct to only *not* search in the global text for new text or should we also search there? Or other globals where searching is discouraged?

- deal with unused functions. Maybe only allow in debug builds? not sure.
