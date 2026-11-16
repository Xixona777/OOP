#!/usr/bin/env python3
from __future__ import annotations

import html
import re
from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_JUSTIFY
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import Paragraph, Preformatted, SimpleDocTemplate

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "theory.md"
TARGET = ROOT / "theory.pdf"


def register_fonts() -> tuple[str, str, str]:
    candidates = [
        (
            Path("C:/Windows/Fonts/arial.ttf"),
            Path("C:/Windows/Fonts/arialbd.ttf"),
            Path("C:/Windows/Fonts/cour.ttf"),
        ),
        (
            Path("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"),
            Path("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"),
            Path("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"),
        ),
    ]
    for regular, bold, mono in candidates:
        if regular.exists() and bold.exists() and mono.exists():
            pdfmetrics.registerFont(TTFont("LabRegular", regular))
            pdfmetrics.registerFont(TTFont("LabBold", bold))
            pdfmetrics.registerFont(TTFont("LabMono", mono))
            pdfmetrics.registerFontFamily(
                "LabRegular",
                normal="LabRegular",
                bold="LabBold",
                italic="LabRegular",
                boldItalic="LabBold",
            )
            return "LabRegular", "LabBold", "LabMono"
    raise FileNotFoundError("Arial or DejaVu fonts with Cyrillic support are required")


def inline_markup(text: str, mono_font: str) -> str:
    escaped = html.escape(text.strip())
    escaped = re.sub(r"\*\*(.+?)\*\*", r"<b>\1</b>", escaped)
    escaped = re.sub(r"`([^`]+)`", rf'<font name="{mono_font}">\1</font>', escaped)
    return escaped


def page_decorations(canvas, document, regular_font: str) -> None:
    canvas.saveState()
    canvas.setFont(regular_font, 8)
    canvas.setFillColor(colors.HexColor("#4b5563"))
    canvas.drawString(18 * mm, A4[1] - 11 * mm, "ООП · Лабораторная работа №4 · Вариант 7")
    canvas.drawRightString(A4[0] - 18 * mm, 10 * mm, f"Страница {document.page}")
    canvas.setStrokeColor(colors.HexColor("#cbd5e1"))
    canvas.line(18 * mm, 14 * mm, A4[0] - 18 * mm, 14 * mm)
    canvas.restoreState()


def build_pdf() -> None:
    regular, bold, mono = register_fonts()
    sample = getSampleStyleSheet()
    body = ParagraphStyle(
        "RussianBody",
        parent=sample["BodyText"],
        fontName=regular,
        fontSize=10,
        leading=14,
        alignment=TA_JUSTIFY,
        spaceAfter=6,
        splitLongWords=True,
    )
    title = ParagraphStyle(
        "RussianTitle",
        parent=body,
        fontName=bold,
        fontSize=21,
        leading=26,
        textColor=colors.HexColor("#12355b"),
        alignment=0,
        spaceBefore=12,
        spaceAfter=12,
    )
    subtitle = ParagraphStyle(
        "RussianSubtitle",
        parent=body,
        fontName=bold,
        fontSize=14,
        leading=18,
        textColor=colors.HexColor("#315b7d"),
        spaceBefore=4,
        spaceAfter=14,
    )
    heading = ParagraphStyle(
        "RussianHeading",
        parent=body,
        fontName=bold,
        fontSize=14,
        leading=18,
        textColor=colors.HexColor("#12355b"),
        spaceBefore=13,
        spaceAfter=7,
        keepWithNext=True,
    )
    bullet = ParagraphStyle(
        "RussianBullet",
        parent=body,
        leftIndent=7 * mm,
        firstLineIndent=0,
        bulletIndent=2 * mm,
        spaceAfter=4,
    )
    code = ParagraphStyle(
        "RussianCode",
        parent=body,
        fontName=mono,
        fontSize=7.4,
        leading=9.4,
        leftIndent=3 * mm,
        rightIndent=3 * mm,
        borderColor=colors.HexColor("#d6dee8"),
        borderWidth=0.5,
        borderPadding=5,
        backColor=colors.HexColor("#f4f7fa"),
        spaceBefore=4,
        spaceAfter=8,
        splitLongWords=True,
    )

    story = []
    paragraph_lines: list[str] = []
    code_lines: list[str] = []
    in_code = False

    def flush_paragraph() -> None:
        if paragraph_lines:
            story.append(Paragraph(inline_markup(" ".join(paragraph_lines), mono), body))
            paragraph_lines.clear()

    def flush_code() -> None:
        if code_lines:
            story.append(Preformatted("\n".join(code_lines), code, maxLineLength=92))
            code_lines.clear()

    for raw_line in SOURCE.read_text(encoding="utf-8").splitlines():
        if raw_line.startswith("```"):
            flush_paragraph()
            if in_code:
                flush_code()
            in_code = not in_code
            continue
        if in_code:
            code_lines.append(raw_line)
            continue
        if not raw_line.strip():
            flush_paragraph()
            continue
        if raw_line.startswith("# "):
            flush_paragraph()
            story.append(Paragraph(inline_markup(raw_line[2:], mono), title))
        elif raw_line.startswith("## "):
            flush_paragraph()
            text = raw_line[3:]
            style = subtitle if text.startswith("Теоретическая часть") else heading
            story.append(Paragraph(inline_markup(text, mono), style))
        elif raw_line.startswith("### "):
            flush_paragraph()
            story.append(Paragraph(inline_markup(raw_line[4:], mono), heading))
        elif raw_line.startswith("- "):
            flush_paragraph()
            story.append(Paragraph(inline_markup(raw_line[2:], mono), bullet, bulletText="•"))
        elif re.match(r"^\d+\.\s", raw_line):
            flush_paragraph()
            match = re.match(r"^(\d+)\.\s+(.*)$", raw_line)
            assert match is not None
            story.append(Paragraph(inline_markup(match.group(2), mono), bullet,
                                   bulletText=f"{match.group(1)}."))
        else:
            paragraph_lines.append(raw_line)

    flush_paragraph()
    flush_code()

    document = SimpleDocTemplate(
        str(TARGET),
        pagesize=A4,
        leftMargin=18 * mm,
        rightMargin=18 * mm,
        topMargin=18 * mm,
        bottomMargin=19 * mm,
        title="Основы метапрограммирования — лабораторная работа №4",
        author="Лабораторная работа по ООП, вариант 7",
        subject="Шаблоны, concepts и умные указатели C++20",
    )
    document.build(
        story,
        onFirstPage=lambda canvas, doc: page_decorations(canvas, doc, regular),
        onLaterPages=lambda canvas, doc: page_decorations(canvas, doc, regular),
    )
    print(f"Created {TARGET}")


if __name__ == "__main__":
    build_pdf()
