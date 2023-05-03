; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 10, i32* %6, align 4
  %8 = load i32, i32* %3, align 4
  %9 = add nsw i32 %8, 10
  store i32 %9, i32* %4, align 4
  %10 = load i32, i32* %3, align 4
  %11 = load i32, i32* %6, align 4
  %12 = add nsw i32 %10, %11
  store i32 %12, i32* %5, align 4
  store i32 30, i32* %7, align 4
  store i32 10, i32* %4, align 4
  br label %13

13:                                               ; preds = %16, %1
  %14 = load i32, i32* %3, align 4
  %15 = icmp slt i32 %14, 100
  br i1 %15, label %16, label %23

16:                                               ; preds = %13
  %17 = load i32, i32* %3, align 4
  %18 = add nsw i32 %17, 5
  store i32 %18, i32* %3, align 4
  %19 = load i32, i32* %6, align 4
  %20 = add nsw i32 %19, 10
  store i32 %20, i32* %6, align 4
  %21 = load i32, i32* %4, align 4
  %22 = add nsw i32 %21, 10
  store i32 %22, i32* %7, align 4
  br label %13, !llvm.loop !6

23:                                               ; preds = %13
  %24 = load i32, i32* %7, align 4
  %25 = icmp sgt i32 %24, 100
  br i1 %25, label %26, label %28

26:                                               ; preds = %23
  %27 = load i32, i32* %6, align 4
  store i32 %27, i32* %2, align 4
  br label %29

28:                                               ; preds = %23
  store i32 100, i32* %2, align 4
  br label %29

29:                                               ; preds = %28, %26
  %30 = load i32, i32* %2, align 4
  ret i32 %30
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
