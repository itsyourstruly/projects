#!/usr/bin/env python3
import sys
import re
from collections import defaultdict, OrderedDict

def format_smart_notes(input_file, output_file):
    """
    Format smart notes consistently every time.
    Handles both new and already-formatted content.
    """
    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Error reading file: {e}", file=sys.stderr)
        sys.exit(1)

    # Use OrderedDict to preserve order of sections
    sections = OrderedDict()
    header_lines = []
    current_section_num = None

    # First pass: identify all sections and their items
    i = 0
    while i < len(lines):
        line = lines[i].rstrip()
        stripped = line.strip()

        # Preserve header/marker lines
        if 'SMART_NOTE' in stripped or stripped.startswith('---') or stripped.startswith('Smart Notes'):
            header_lines.append(line)
            i += 1
            continue

        # Skip completely empty lines during parsing
        if not stripped:
            i += 1
            continue

        # Pattern 1: "1a. Item" - numbered bullet (number + letter + dot)
        numbered_bullet = re.match(r'^(\d+)([a-z])\.\s*(.*)$', stripped)

        # Pattern 2: "1. Title" - section title (number + dot, with or without text)
        section_title = re.match(r'^(\d+)\.\s*(.*)$', stripped)

        # Pattern 3: "a. Item" - standalone letter bullet
        letter_bullet = re.match(r'^([a-z])\.\s*(.*)$', stripped)

        if numbered_bullet:
            # "1a. Item" format
            section_num, letter, text = numbered_bullet.groups()
            if section_num not in sections:
                sections[section_num] = {'title': '', 'items': []}
            sections[section_num]['items'].append((letter, text.strip()))
            current_section_num = section_num

        elif section_title and not numbered_bullet:
            # "1. Title" format (but not "1a. Item")
            section_num, title = section_title.groups()
            if section_num not in sections:
                sections[section_num] = {'title': title.strip(), 'items': []}
            else:
                # Update title if this section already exists
                sections[section_num]['title'] = title.strip()
            current_section_num = section_num

        elif letter_bullet:
            # "a. Item" format - add to current or last section
            letter, text = letter_bullet.groups()

            # Determine which section to add to
            target_section = None

            if current_section_num:
                # Add to current section
                target_section = current_section_num
            elif sections:
                # Add to the last section (highest number)
                target_section = list(sections.keys())[-1]

            if target_section:
                if target_section not in sections:
                    sections[target_section] = {'title': '', 'items': []}
                sections[target_section]['items'].append((letter, text.strip()))

        # If we didn't match any pattern, keep it as header
        elif stripped and not any(p in stripped for p in ['Example', 'Becomes:', 'Type', 'Sub-items']):
            # Only add non-empty, non-instructional lines as header
            if stripped and len(stripped) > 0:
                header_lines.append(line)

        i += 1

    # Build formatted output
    output_lines = []

    # Add header lines
    for line in header_lines:
        output_lines.append(line)

    if header_lines:
        output_lines.append('')  # Blank line after header

    # Add sections in numeric order
    sorted_sections = sorted(sections.items(), key=lambda x: int(x[0]))

    for section_num, section_data in sorted_sections:
        # Add section title with number
        title = section_data['title']
        if title:
            output_lines.append(f"{section_num}. {title}")
        else:
            output_lines.append(f"{section_num}.")

        # Sort items alphabetically by letter, preserving order for duplicates
        items = section_data['items']
        sorted_items = sorted(items, key=lambda x: (x[0], items.index(x)))

        # Add items with proper indentation
        for letter, text in sorted_items:
            if text:  # Only add items with content
                output_lines.append(f"  {letter}. {text}")

        output_lines.append('')  # Blank line after each section

    # Write output
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(output_lines))
            if output_lines:  # Add final newline
                f.write('\n')
    except Exception as e:
        print(f"Error writing file: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: smart_format.py <input> <output>")
        sys.exit(1)

    format_smart_notes(sys.argv[1], sys.argv[2])

