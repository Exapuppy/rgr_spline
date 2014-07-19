rgr_spline
==========

РГР по курсу "Компьютерная графика".

Тема:
Интерактивное создание кривых и поверхностей с использованием сплайнов.
Дифференцирование с использованием кусочно-кубического эрмитова сглаживающего сплайна.

Интерфейс реализован на OpenGL.
Входными данными является набор точек на экране, задаваемый в произвольном порядке с помощью мыши. Исходный набор точек также отображается. При добавлении новой точки сплайн автоматически пересчитывается.
Предусмотрен коэффициент β при вторых производных сплайна и - для изменения первых производных. Последствием нажатий правой кнопки мыши ось абсцисс разбивается на конечные элементы, которые визуализируются вертикальными линиями. По умолчанию задан один конечный элемент. Программа включает навигацию по сцене, масштабирование с помощью клавиатуры. Выводится координатная сетка с клетками единичного размера.
