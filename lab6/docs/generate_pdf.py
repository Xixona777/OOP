#!/usr/bin/env python3
from pathlib import Path
import html
import re

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_JUSTIFY
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    PageBreak,
    Paragraph,
    SimpleDocTemplate,
    Spacer,
    Preformatted,
)

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "theory.md"
TARGET = ROOT / "theory.pdf"


def register_fonts():
    candidates = []
    try:
        import matplotlib

        candidates.append((
            Path(matplotlib.get_data_path()) / "fonts/ttf",
            "DejaVuSans.ttf",
            "DejaVuSans-Bold.ttf",
            "DejaVuSans-Oblique.ttf",
            "DejaVuSansMono.ttf",
        ))
    except ImportError:
        pass
    candidates.extend([
        (Path("/usr/share/fonts/truetype/dejavu"), "DejaVuSans.ttf", "DejaVuSans-Bold.ttf", "DejaVuSans-Oblique.ttf", "DejaVuSansMono.ttf"),
        (Path("C:/Windows/Fonts"), "arial.ttf", "arialbd.ttf", "ariali.ttf", "cour.ttf"),
        (Path("/usr/share/fonts/truetype/liberation2"), "LiberationSans-Regular.ttf", "LiberationSans-Bold.ttf", "LiberationSans-Italic.ttf", "LiberationMono-Regular.ttf"),
    ])
    for directory, regular, bold, italic, mono in candidates:
        paths = [directory / name for name in (regular, bold, italic, mono)]
        if all(path.exists() for path in paths):
            pdfmetrics.registerFont(TTFont("TheoryRegular", str(paths[0])))
            pdfmetrics.registerFont(TTFont("TheoryBold", str(paths[1])))
            pdfmetrics.registerFont(TTFont("TheoryItalic", str(paths[2])))
            pdfmetrics.registerFont(TTFont("TheoryMono", str(paths[3])))
            pdfmetrics.registerFontFamily(
                "TheoryRegular",
                normal="TheoryRegular",
                bold="TheoryBold",
                italic="TheoryItalic",
                boldItalic="TheoryBold",
            )
            return
    raise RuntimeError("No Unicode TrueType fonts with Cyrillic support were found")


def inline_markup(text):
    escaped = html.escape(text.strip())
    escaped = re.sub(
        r"`([^`]+)`",
        r'<font name="TheoryMono" color="#17365d">\1</font>',
        escaped,
    )
    escaped = re.sub(r"\*\*([^*]+)\*\*", r"<b>\1</b>", escaped)
    return escaped


def footer(canvas, doc):
    canvas.saveState()
    canvas.setFont("TheoryRegular", 8)
    canvas.setFillColor(colors.HexColor("#555555"))
    canvas.drawString(18 * mm, 10 * mm, "Лабораторная работа №6 — паттерны проектирования")
    canvas.drawRightString(A4[0] - 18 * mm, 10 * mm, f"Страница {doc.page}")
    canvas.setStrokeColor(colors.HexColor("#bbbbbb"))
    canvas.line(18 * mm, 13 * mm, A4[0] - 18 * mm, 13 * mm)
    canvas.restoreState()


def make_styles():
    sample = getSampleStyleSheet()
    return {
        "title": ParagraphStyle(
            "TheoryTitle",
            parent=sample["Title"],
            fontName="TheoryBold",
            fontSize=22,
            leading=28,
            alignment=TA_CENTER,
            textColor=colors.HexColor("#17365d"),
            spaceAfter=10 * mm,
        ),
        "subtitle": ParagraphStyle(
            "TheorySubtitle",
            parent=sample["Heading2"],
            fontName="TheoryBold",
            fontSize=14,
            leading=19,
            alignment=TA_CENTER,
            textColor=colors.HexColor("#385d8a"),
            spaceAfter=12 * mm,
        ),
        "h1": ParagraphStyle(
            "TheoryH1",
            parent=sample["Heading1"],
            fontName="TheoryBold",
            fontSize=15,
            leading=20,
            textColor=colors.HexColor("#17365d"),
            spaceBefore=7 * mm,
            spaceAfter=3 * mm,
            keepWithNext=True,
        ),
        "h2": ParagraphStyle(
            "TheoryH2",
            parent=sample["Heading2"],
            fontName="TheoryBold",
            fontSize=12,
            leading=16,
            textColor=colors.HexColor("#385d8a"),
            spaceBefore=5 * mm,
            spaceAfter=2 * mm,
            keepWithNext=True,
        ),
        "body": ParagraphStyle(
            "TheoryBody",
            parent=sample["BodyText"],
            fontName="TheoryRegular",
            fontSize=10.0,
            leading=14.0,
            alignment=TA_JUSTIFY,
            spaceAfter=2.5 * mm,
            splitLongWords=True,
        ),
        "bullet": ParagraphStyle(
            "TheoryBullet",
            parent=sample["BodyText"],
            fontName="TheoryRegular",
            fontSize=10.0,
            leading=14.0,
            leftIndent=7 * mm,
            firstLineIndent=-4 * mm,
            spaceAfter=1.5 * mm,
        ),
        "code": ParagraphStyle(
            "TheoryCode",
            parent=sample["Code"],
            fontName="TheoryMono",
            fontSize=7.7,
            leading=10.2,
            leftIndent=4 * mm,
            rightIndent=3 * mm,
            borderColor=colors.HexColor("#c7d5e5"),
            borderWidth=0.5,
            borderPadding=5,
            backColor=colors.HexColor("#f3f6f9"),
            spaceBefore=2 * mm,
            spaceAfter=4 * mm,
        ),
    }


def markdown_to_story(markdown, styles):
    story = []
    paragraph_lines = []
    code_lines = []
    in_code = False
    title_seen = False

    def flush_paragraph():
        nonlocal paragraph_lines
        if paragraph_lines:
            text = " ".join(line.strip() for line in paragraph_lines)
            story.append(Paragraph(inline_markup(text), styles["body"]))
            paragraph_lines = []

    for raw_line in markdown.splitlines():
        line = raw_line.rstrip()
        if line.strip().startswith("```"):
            flush_paragraph()
            if in_code:
                code = "\n".join(code_lines)
                story.append(Preformatted(code, styles["code"], maxLineLength=88))
                code_lines = []
                in_code = False
            else:
                in_code = True
            continue
        if in_code:
            code_lines.append(line)
            continue
        if not line.strip():
            flush_paragraph()
            continue
        heading = re.match(r"^(#{1,3})\s+(.+)$", line)
        if heading:
            flush_paragraph()
            level = len(heading.group(1))
            text = inline_markup(heading.group(2))
            if level == 1 and not title_seen:
                story.append(Paragraph(text, styles["title"]))
                title_seen = True
            elif level == 1:
                story.append(Paragraph(text, styles["h1"]))
            else:
                style = styles["subtitle"] if heading.group(2).startswith("Теоретическая часть") else styles["h2"]
                story.append(Paragraph(text, style))
            continue
        bullet = re.match(r"^[-*]\s+(.+)$", line)
        if bullet:
            flush_paragraph()
            story.append(Paragraph("• " + inline_markup(bullet.group(1)), styles["bullet"]))
            continue
        numbered = re.match(r"^(\d+\.)\s+(.+)$", line)
        if numbered:
            flush_paragraph()
            story.append(Paragraph(inline_markup(numbered.group(1) + " " + numbered.group(2)), styles["bullet"]))
            continue
        paragraph_lines.append(line)

    flush_paragraph()
    if in_code:
        raise RuntimeError("Unclosed code block in theory.md")
    return story


def main():
    register_fonts()
    document = SimpleDocTemplate(
        str(TARGET),
        pagesize=A4,
        rightMargin=18 * mm,
        leftMargin=18 * mm,
        topMargin=17 * mm,
        bottomMargin=18 * mm,
        title="Паттерны проектирования в редакторе подземелья",
        author="Лабораторная работа №6 по ООП",
        subject="Factory, Visitor, Observer и SOLID",
    )
    story = markdown_to_story(SOURCE.read_text(encoding="utf-8"), make_styles())
    document.build(story, onFirstPage=footer, onLaterPages=footer)
    print(f"Created {TARGET}")


if __name__ == "__main__":
    main()
