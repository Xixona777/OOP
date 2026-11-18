#!/usr/bin/env python3

from __future__ import annotations

import html
import re
from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER
from reportlab.lib.fonts import addMapping
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    Paragraph,
    SimpleDocTemplate,
    Spacer,
    Table,
    TableStyle,
    XPreformatted,
)

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "theory.md"
OUTPUT = ROOT / "theory.pdf"


def register_fonts() -> None:
    candidates = [
        (
            Path("C:/Windows/Fonts"),
            {
                "Arial": "arial.ttf",
                "Arial-Bold": "arialbd.ttf",
                "Arial-Italic": "ariali.ttf",
                "CourierNew": "cour.ttf",
                "CourierNew-Bold": "courbd.ttf",
            },
        ),
        (
            Path("/usr/share/fonts/truetype/dejavu"),
            {
                "Arial": "DejaVuSans.ttf",
                "Arial-Bold": "DejaVuSans-Bold.ttf",
                "Arial-Italic": "DejaVuSans-Oblique.ttf",
                "CourierNew": "DejaVuSansMono.ttf",
                "CourierNew-Bold": "DejaVuSansMono-Bold.ttf",
            },
        ),
    ]
    for directory, fonts in candidates:
        if all((directory / filename).exists() for filename in fonts.values()):
            for name, filename in fonts.items():
                pdfmetrics.registerFont(TTFont(name, str(directory / filename)))
            addMapping("Arial", 0, 0, "Arial")
            addMapping("Arial", 1, 0, "Arial-Bold")
            addMapping("Arial", 0, 1, "Arial-Italic")
            addMapping("CourierNew", 1, 0, "CourierNew-Bold")
            return
    raise RuntimeError("Arial or DejaVu fonts with Cyrillic support were not found")


def inline_markup(text: str) -> str:
    value = html.escape(text.strip())
    value = re.sub(
        r"`([^`]+)`",
        r'<font name="CourierNew" size="9" color="#17365d">\1</font>',
        value,
    )
    value = re.sub(r"\*\*([^*]+)\*\*", r"<b>\1</b>", value)
    value = re.sub(r"\[([^]]+)\]\([^)]+\)", r"\1", value)
    return value


def wrap_code(code: str, limit: int = 96) -> str:
    wrapped: list[str] = []
    for line in code.rstrip().splitlines():
        if len(line) <= limit:
            wrapped.append(line)
            continue
        while len(line) > limit:
            wrapped.append(line[:limit] + " \\")
            line = "    " + line[limit:]
        wrapped.append(line)
    return "\n".join(wrapped)


def footer(canvas, document) -> None:
    canvas.saveState()
    canvas.setFont("Arial", 8)
    canvas.setFillColor(colors.HexColor("#555555"))
    canvas.drawString(20 * mm, 11 * mm, "Лабораторная работа №7 — асинхронное программирование")
    canvas.drawRightString(A4[0] - 20 * mm, 11 * mm, f"Страница {document.page}")
    canvas.restoreState()


def build() -> None:
    register_fonts()
    base = getSampleStyleSheet()
    body = ParagraphStyle(
        "BodyRussian",
        parent=base["BodyText"],
        fontName="Arial",
        fontSize=10.2,
        leading=14.5,
        spaceAfter=6,
        textColor=colors.HexColor("#202020"),
        allowWidows=0,
        allowOrphans=0,
    )
    title = ParagraphStyle(
        "TitleRussian",
        parent=body,
        fontName="Arial-Bold",
        fontSize=21,
        leading=27,
        alignment=TA_CENTER,
        textColor=colors.HexColor("#17365d"),
        spaceBefore=30,
        spaceAfter=18,
    )
    subtitle = ParagraphStyle(
        "SubtitleRussian",
        parent=body,
        fontName="Arial-Bold",
        fontSize=14,
        leading=19,
        alignment=TA_CENTER,
        textColor=colors.HexColor("#365f91"),
        spaceAfter=18,
    )
    heading1 = ParagraphStyle(
        "Heading1Russian",
        parent=body,
        fontName="Arial-Bold",
        fontSize=15,
        leading=19,
        textColor=colors.HexColor("#17365d"),
        spaceBefore=14,
        spaceAfter=8,
        keepWithNext=True,
    )
    heading2 = ParagraphStyle(
        "Heading2Russian",
        parent=body,
        fontName="Arial-Bold",
        fontSize=12,
        leading=16,
        textColor=colors.HexColor("#365f91"),
        spaceBefore=10,
        spaceAfter=6,
        keepWithNext=True,
    )
    bullet = ParagraphStyle(
        "BulletRussian",
        parent=body,
        leftIndent=15,
        bulletIndent=2,
        firstLineIndent=0,
        spaceAfter=4,
    )
    code_style = ParagraphStyle(
        "CodeRussian",
        parent=body,
        fontName="CourierNew",
        fontSize=7.8,
        leading=10.2,
        leftIndent=5,
        rightIndent=5,
        borderColor=colors.HexColor("#c8d6e5"),
        borderWidth=0.5,
        borderPadding=6,
        backColor=colors.HexColor("#f4f7fa"),
        spaceBefore=4,
        spaceAfter=8,
    )
    table_text = ParagraphStyle(
        "TableTextRussian",
        parent=body,
        fontSize=8.5,
        leading=11,
        spaceAfter=0,
    )
    table_header = ParagraphStyle(
        "TableHeaderRussian",
        parent=table_text,
        fontName="Arial-Bold",
        textColor=colors.white,
    )

    document = SimpleDocTemplate(
        str(OUTPUT),
        pagesize=A4,
        leftMargin=20 * mm,
        rightMargin=20 * mm,
        topMargin=18 * mm,
        bottomMargin=20 * mm,
        title="Асинхронное программирование в C++20",
        subject="Лабораторная работа №7 по ООП, вариант 10",
        author="",
        creator="ReportLab",
    )
    content_width = A4[0] - document.leftMargin - document.rightMargin
    story = []
    lines = SOURCE.read_text(encoding="utf-8").splitlines()
    index = 0
    first_h2 = True

    while index < len(lines):
        line = lines[index]
        stripped = line.strip()
        if not stripped:
            index += 1
            continue

        if stripped.startswith("```"):
            index += 1
            code_lines = []
            while index < len(lines) and not lines[index].strip().startswith("```"):
                code_lines.append(lines[index])
                index += 1
            index += 1
            story.append(XPreformatted(wrap_code("\n".join(code_lines)), code_style))
            continue

        if stripped.startswith("|"):
            table_lines = []
            while index < len(lines) and lines[index].strip().startswith("|"):
                table_lines.append(lines[index].strip())
                index += 1
            rows = []
            for row_index, table_line in enumerate(table_lines):
                cells = [cell.strip() for cell in table_line.strip("|").split("|")]
                if row_index == 1 and all(re.fullmatch(r":?-{3,}:?", cell) for cell in cells):
                    continue
                style = table_header if not rows else table_text
                rows.append([Paragraph(inline_markup(cell), style) for cell in cells])
            columns = len(rows[0])
            table = Table(rows, colWidths=[content_width / columns] * columns,
                          repeatRows=1, hAlign="LEFT")
            table.setStyle(TableStyle([
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#365f91")),
                ("BACKGROUND", (0, 1), (-1, -1), colors.HexColor("#f4f7fa")),
                ("GRID", (0, 0), (-1, -1), 0.4, colors.HexColor("#a6b7c8")),
                ("VALIGN", (0, 0), (-1, -1), "TOP"),
                ("LEFTPADDING", (0, 0), (-1, -1), 5),
                ("RIGHTPADDING", (0, 0), (-1, -1), 5),
                ("TOPPADDING", (0, 0), (-1, -1), 5),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 5),
            ]))
            story.extend([table, Spacer(1, 8)])
            continue

        if stripped.startswith("# "):
            story.append(Paragraph(inline_markup(stripped[2:]), title))
            index += 1
            continue
        if stripped.startswith("## "):
            style = subtitle if first_h2 else heading1
            first_h2 = False
            story.append(Paragraph(inline_markup(stripped[3:]), style))
            index += 1
            continue
        if stripped.startswith("### "):
            story.append(Paragraph(inline_markup(stripped[4:]), heading2))
            index += 1
            continue

        bullet_match = re.match(r"^-\s+(.+)$", stripped)
        if bullet_match:
            story.append(Paragraph(inline_markup(bullet_match.group(1)), bullet,
                                   bulletText="•"))
            index += 1
            continue

        paragraph_lines = [stripped]
        index += 1
        while index < len(lines):
            next_line = lines[index].strip()
            if (not next_line or next_line.startswith(("#", "```", "|", "- "))
                    or re.match(r"^\d+\.\s", next_line)):
                break
            paragraph_lines.append(next_line)
            index += 1
        markup = "<br/>".join(inline_markup(part.removesuffix("  "))
                              for part in paragraph_lines)
        story.append(Paragraph(markup, body))

    document.build(story, onFirstPage=footer, onLaterPages=footer)
    print(f"Created {OUTPUT}")


if __name__ == "__main__":
    build()
