// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"

int32 UInv_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;
}

FIntPoint UInv_WidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index%Columns, Index/Columns);
}

bool UInv_WidgetUtils::IsWithinBounds(const FVector2D& BoundaryPos, const FVector2D& WidgetSize,
	const FVector2D& MousePos)
{
	return MousePos.X >= BoundaryPos.X && MousePos.X <= (BoundaryPos.X + WidgetSize.X) &&
		MousePos.Y >= BoundaryPos.Y && MousePos.Y <= (BoundaryPos.Y + WidgetSize.Y);
}

FVector2D UInv_WidgetUtils::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	const FVector2D LocalPosition = USlateBlueprintLibrary::GetLocalTopLeft(Geometry);
	
	USlateBlueprintLibrary::LocalToViewport(Widget,Geometry,LocalPosition,PixelPosition,ViewportPosition);
	
	return ViewportPosition;
}

FVector2D UInv_WidgetUtils::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

FVector2D UInv_WidgetUtils::GetClampedWidgetPosition(const FVector2D& Boundary, const FVector2D& WidgetSize,
	const FVector2D& MousePos)
{
	FVector2D ClampedPosition = MousePos;
	//Adjust the horizontal position to ensure that the widget stays within the boundary
	/*Widget exceeds the right edge*/
	if (MousePos.X + WidgetSize.X > Boundary.X)
	{
		ClampedPosition.X = Boundary.X - WidgetSize.X;
	}
	/*Widget exceeds the left edge*/
	if (MousePos.X < 0.f)
	{
		ClampedPosition.X = 0.f;
	}
	
	//Adjust the vertical position to ensure that the widget stays within the boundary
	
	/*Widget exceeds the bottom edge*/
	if (MousePos.Y + WidgetSize.Y > Boundary.Y)
	{
		ClampedPosition.Y = Boundary.Y - WidgetSize.Y;
	}
	/*Widget exceeds the upper edge*/
	if (MousePos.Y < 0.f)
	{
		ClampedPosition.Y = 0.f;
	}
	
	return ClampedPosition;
}