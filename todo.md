- Rewrite parsing method.
    1. Parse tag name
    2. Add any attributes
    3. Set TagID for that nodeID, be it single or paired.
- Add in contexts of <style> and <script> tags.
    - style tags don't care about anything, everything is text until u specifically encounter </style>
    - <script> tags don't care about anything that is within quotes, is all considered text.
- Instead of hardcoded 8 for bits in byte we can use define that is defined somewhere.
- Deleting/Modifying global elements
    - E.g., when we merge text nodes during parsing.

- Is it correct to only *not* search in the global text for new text or should we also search there? Or other globals where searching is discouraged?

- deal with unused functions. Maybe only allow in debug builds? not sure.
